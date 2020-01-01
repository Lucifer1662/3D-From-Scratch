#include <RenderMesh.h>
#include <vector>
#include <glm\glm.hpp>
#include <iostream>
#include <functional>
#include <glm\gtx\transform.hpp>
#include <amp.h> 
using namespace concurrency;
using uint = unsigned int;
using glm::vec2;
using glm::vec3;

using vec4ui = glm::tvec4<uint, glm::precision::mediump>;
using vec2i = glm::tvec2<int, glm::precision::mediump>;
using rectui = vec4ui;
using vec2ui = glm::tvec2<uint, glm::precision::mediump>;

#define At(x,y) 600*y+x

namespace Window {
	extern vector<vec3> pixels;
	extern vec2ui windowSize;
	extern rectui viewPort;
	extern rectui viewPortHalf;

	template<typename S>
	class Shader {
	public:
		S frag;
		vec2i origin;
		float gradientStart;
		float gradientEnd;
		array_view<vec3, 2> arrayView;


		Shader(vec2i origin, float gradientStart, float gradientEnd, array_view<vec3, 2> arrayView, S frag)
			: origin(origin), gradientStart(gradientStart), gradientEnd(gradientEnd), arrayView(arrayView), frag(frag)
		{}


		void operator()(index<2> idx) restrict(amp) {
		
			int x = idx[1]-origin.x;
			int y = idx[0]-origin.y;
			if (x >= y*gradientStart && x <= y*gradientEnd) {
				frag(arrayView[idx]);
			}
		}


	};


//   v0
//  /  \
// v1__v2

// v1__v2
//  \  /
//	 v0

	template<typename S>
	void DrawTriangle(const S& func, vec2 v0, vec2 v1, vec2 v2, bool isBottomSide) {
		float gradientStart = (v1.x - v0.x) / (v1.y - v0.y);
		float gradientEnd = (v2.x - v0.x) / (v2.y - v0.y);

		if (!isBottomSide) {
			float temp = v1.y;
			v1.y = v0.y;
			v0.y = temp;
		}

		bool isLeft;
		if (isLeft = (v0.x < v1.x)) {
			float temp = v1.x;
			v1.x = v0.x;
			v0.x = temp;
		}
		else
			if (v0.x > v2.x) {
				v2.x = v0.x;
			}
		int width = (int)ceil(v2.x - v1.x - 0.5f);
		if (width <= 0)
			return;

		int height = (int)ceil(v0.y - v1.y - 0.5f);
		if (height <= 0)
			return;

	
	

		int x = (int)ceil(v1.x - 0.5f);
		if (x <= 0)
		{
			x = 0;
			width += v1.x;
		}
		if (x >= viewPort.z)
			return;

		int y = (int)ceil(v1.y - 0.5f);
		if (y <= 0) {
			y = 0;
			height += v0.y;
		}
		if (y >= viewPort.w)
			return;

		array_view<vec3, 2> arrayView(viewPort.w, viewPort.z, pixels);
		arrayView = arrayView.section(y, x, min(max(height, 0), viewPort.w - y), min(max(width, 0), viewPort.z - x));



		accelerator device = accelerator(accelerator::default_accelerator);
		accelerator_view view = device.default_view;



		Shader<decltype(fun)> s = (isLeft) ?
			Shader<decltype(fun)>({ (v1.x > 0) ? 0 : v1.x, isBottomSide? height: 0 }, gradientStart, gradientEnd, arrayView, fun) :
			Shader<decltype(fun)>({ (v1.x > 0) ? (v0.x - v1.x) : v0.x, isBottomSide? height: 0 }, gradientStart, gradientEnd, arrayView, fun);


		parallel_for_each(view, arrayView.extent, s);


	}


	template<typename F1>
	void RasteriseTriangle(const F1& fragmentShader, vec3 v0, vec3 v1, vec3 v2) {		

		const vec2* _v0 = (vec2*)&v0;
		const vec2* _v1 = (vec2*)&v1;
		const vec2* _v2 = (vec2*)&v2;


		//sort by y
		if (_v0->y < _v1->y) std::swap(_v1, _v0);
		//so now v0 > v1
		if (_v1->y < _v2->y) {
			std::swap(_v1, _v2);
			//if this happens then we need to check v0 again with the new v1
			if (_v0->y < _v1->y) std::swap(_v1, _v0);
		}
		//so v0 > v1 > v2 for there y's
		if (_v0->y == _v1->y) {
			//now it must look like
			// v0-----v1
			// \   /
			//	 v2
			if (_v0->x > _v1->x) std::swap(_v1, _v0);
			//Top
			DrawTriangle(fragmentShader, *_v2, *_v0, *_v1, false);
		}
		else if (_v2->y == _v1->y) {
			//now it must look like
			//   v0
			// /   \
			//v1-----v2
			if (_v1->x > _v2->x) std::swap(_v1, _v2);
			//bottom
			DrawTriangle(fragmentShader, *_v0, *_v1, *_v2, true);
		}
		else {
			//it is a weird one
			// v0			  v0
			//		v1  or v1
			//v2				  v2
			//since v1 is in the middle it is the splitter to make two new triangles

			float alpha = (_v1->y - _v0->y) / (_v2->y - _v0->y);
			vec2 vi = *_v0 + (*_v2 - *_v0)*alpha;

			//  v0		
			// vi	v1  
			//v2		
			if (vi.x < _v1->x) {
				//bottom
				DrawTriangle(fragmentShader, *_v0, vi, *_v1, true);
				//top
				DrawTriangle(fragmentShader, *_v2, vi, *_v1, false);
			}

			//    v0		
			// v1	vi  
			//        v2	
			else {
				//bottom
				DrawTriangle(fragmentShader, *_v0, *_v1, vi, true);
				//top
				DrawTriangle(fragmentShader, *_v2, *_v1, vi, false);
			}

		}

	}

	template<typename VS, typename FS>
	void DrawToScreen(Mesh mesh, VS& vertShader, FS& fragShader) {
		auto end = &mesh.indexes.back() + 1;
		vec3* vertices = &mesh.vertices[0];

		vector<vec3> newVerts;
		newVerts.reserve(mesh.indexes.size());

		for (auto i = &mesh.indexes.front(); i < end; i ++) {
			vec3 vert = vertShader(vec4(vertices[*i], 1));
			vert.x = (vert.x + 1) * viewPortHalf.z;
			vert.y = (vert.y + 1) * viewPortHalf.w;
			newVerts.emplace_back(vert);
		}
		auto newVertEnd = &newVerts.back();
		for (auto vert = &newVerts.front(); vert < newVertEnd; vert += 3)
			RasteriseTriangle(fragShader, *vert, *(vert + 1), *(vert + 2));


	}
	void ChangeScreenSize(uint width, uint height);
	void Create(int width, int height, const char* windowName, function<void()> startFunction, function<void()> updateFunction);



}
