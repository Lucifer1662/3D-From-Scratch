#include <Window.h>
#include <glm/gtx/transform.hpp>
#include <iostream>
#include <time.h>

using namespace Window;
using glm::mat4;
using glm::vec3;

mat4 modelMatrix;
Mesh cube;
struct ModelMatrixVS {

	mat4 modelMatrix;
	vec3 operator() (vec4 vertex) {
		//std::cout << vertex.x << " " << vertex.y << " " << vertex.z << std::endl;
		vertex = modelMatrix * vertex;
		//std::cout << vertex.x << " " << vertex.y << " " << vertex.z << std::endl;
		vertex.z += 1;
		//vertex = vertex * camera;
		vertex.x /= vertex.z;
		vertex.y /= vertex.z;
		return vertex;
	}

}vs;

struct ColourShader {
	vec3 colour;
	void operator()(vec3& colour) restrict(amp) {
		colour.x = this->colour.x;
		colour.y = this->colour.y;
		colour.z = this->colour.z;

	}
}fun;



void Start() {
	//1 3
	//0 2

	//5 7
	//4 6 
	cube.vertices = {
		{ -0.5f,-0.5f,0.5f },
		{ -0.5f, 0.5f,0.5f },
		{ 0.5f,-0.5f,0.5f },
		{ 0.5f, 0.5f,0.5f },

		{ -0.5f,-0.5f,-0.5f },
		{ -0.5f, 0.5f,-0.5f },
		{ 0.5f,-0.5f,-0.5f },
		{ 0.5f, 0.5f,-0.5f }
	};

	cube.indexes = {
		0,1,2,
		2,3,1,

		0,1,5,
		5,4,0

	};




	 

	/*vertexShader = [](vec4& vertex) {

		//std::cout << "Before" << std::endl;
		std::cout << vertex.x << " " << vertex.y << " " << vertex.z << std::endl;
		vertex = modelMatrix * vertex;
		std::cout << vertex.x << " " << vertex.y << " " << vertex.z << std::endl;
		vertex.z += 1;
		//vertex = vertex * camera;
		vertex.x /= vertex.z;
		vertex.y /= vertex.z;

	};
	*/
	
	fun.colour = vec3(1, 0, 0);
}

float t = 0;

void Update() {
	auto start = clock();
	
	vs.modelMatrix = glm::translate(vec3(0.0f, 0.0f, 1.5f)) * glm::rotate(glm::radians(0.0f), vec3(0.0f, 0.0f, 1)) * glm::rotate(glm::radians(t*50.0f), vec3(0.0f, 1.0f, 0));

	DrawToScreen(cube,vs,fun);
	//modelMatrix = glm::translate(vec3(t, t, 0));
	auto end = clock();
	float dif = (float)(end - start)/ CLOCKS_PER_SEC;
	t += dif;
	//std::cout << '\r' << 1.0f / dif;
	std::cout << std::to_string(t) << std::endl;
	
}

void main() {
	Create(600, 600, "3dFromSratch", Start, Update);

}


/*
#define GLEW_STATIC
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <vector>
#include <glm\glm.hpp>
#include <iostream>
#include <functional>
#include <glm\gtx\transform.hpp>
using std::vector;
using glm::vec4;
using glm::vec3;
using glm::vec2;
using std::function;

using uint = unsigned int;
using vec4ui = glm::tvec4<uint, glm::precision::mediump>;
using rectui = vec4ui;
using vec2ui = glm::tvec2<uint, glm::precision::mediump>;











vector<vec3> pixels;


vec2ui screenSize = {600,600};
rectui viewPort = { 0,0,screenSize.x,screenSize.y};
rectui viewPortHalf = viewPort / 2u;



#define At(x,y) 600*y+x
//   v0
//  /  \
// v1__v2	 
template<typename Function>
void DrawBottomTriangleToScreen(const Function& func, const vec2& v0, const vec2& v1, const vec2& v2) {
	float gradientStart = (v1.x - v0.x) / (v1.y - v0.y);
	float gradientEnd = (v2.x - v0.x) / (v2.y - v0.y);

	
	

	for (size_t y = v1.y; y < v0.y; y++)
	{
		//when alpha is 0 i want the start to be at v1.x
		//when alpha is 1 i want the start to be at v0.x
		//the gradient is run over rise
		int start = v1.x + (y - v1.y)*gradientStart;

		int end = v2.x + (y - v1.y)*gradientEnd;
		for (size_t x = start; x < end; x++)
		{
			func(pixels[At(x, y)]);
		}
	}
}

// v1__v2
//  \  /
//	 v0
template<typename Function>
void DrawTopTriangleToScreen(const Function& func, const vec2& v0, const vec2& v1, const vec2& v2) {
	float gradientStart = (v1.x - v0.x) / (v1.y - v0.y);
	float gradientEnd = (v2.x - v0.x) / (v2.y - v0.y);

	for (size_t y = v0.y; y < v1.y; y++)
	{
		//when alpha is 0 i want the start to be at v1.x
		//when alpha is 1 i want the start to be at v0.x
		//the gradient is run over rise
		int start = v1.x + (y - v1.y)*gradientStart;

		int end = v2.x + (y - v1.y)*gradientEnd;
		for (size_t x = start; x < end; x++)
		{
			func(pixels[At(x, y)]);
		}
	}
}

template<typename F1, typename F2>
void RasteriseTriangle(const F1& vertexShader, const F2& fragmentShader, vec4& v0, vec4& v1, vec4& v2) {
	//-1
	//0 = 300
	//1 = 600
	vertexShader(v0);
	vertexShader(v1);
	vertexShader(v2);

	v0.x = (v0.x + 1) * viewPortHalf.z;
	v0.y = (v0.y + 1) * viewPortHalf.w;

	v1.x = (v1.x + 1) * viewPortHalf.z;
	v1.y = (v1.y + 1) * viewPortHalf.w;

	v2.x = (v2.x + 1) * viewPortHalf.z;
	v2.y = (v2.y + 1) * viewPortHalf.w;

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
		DrawTopTriangleToScreen(fragmentShader, *_v2, *_v0, *_v1);
	}
	else if (_v2->y == _v1->y) {
		//now it must look like
		//   v0
		// /   \
		//v1-----v2
		if (_v1->x > _v2->x) std::swap(_v1, _v2);
		DrawBottomTriangleToScreen(fragmentShader, *_v0, *_v1, *_v2);
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
			DrawBottomTriangleToScreen(fragmentShader, *_v0, vi, *_v1);
			DrawTopTriangleToScreen(fragmentShader, *_v2, vi, *_v1);
		}

		//    v0		
		// v1	vi  
		//        v2	
		else {
			DrawBottomTriangleToScreen(fragmentShader, *_v0, *_v1, vi);
			DrawTopTriangleToScreen(fragmentShader, *_v2, *_v1, vi);
		}

	}

}



void DrawToScreen(RenderMesh renderMesh) {
	auto end = &renderMesh.mesh.indexes.back();
	const vec3* vertices = &renderMesh.mesh.vertices[0];

	for (auto i = &renderMesh.mesh.indexes.front(); i < end; i+=3)
	{
		RasteriseTriangle(renderMesh.vertexShader, renderMesh.fragmentShader, vec4(vertices[*i],1), vec4(vertices[*(i+1)],1), vec4(vertices[*(i+2)],1));
	}	
}

void main() {
	int a = glewInit();
	int b = glfwInit();
	
	auto window = glfwCreateWindow(600, 600, "3DFromScrath", NULL, NULL);
	glfwMakeContextCurrent(window);
	pixels.resize(600 * 600);

	std::function<void(vec3&)> fragmentShader = [](vec3& pixel) {
		pixel = vec3(1, 0, 0);
	};


	

	glm::mat4 camera = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 30.0f);
	glm::mat4 modelMatrix = glm::translate(vec3(3, 3, 3));
	

	std::function<void(vec4&)> vertexShader = [&modelMatrix](vec4& vertex) {

		//std::cout << "Before" << std::endl;
		std::cout << vertex.x << " " << vertex.y << " " << vertex.z << std::endl;
		vertex = modelMatrix * vertex;		
		std::cout << vertex.x << " " << vertex.y << " " << vertex.z << std::endl;
		vertex.z += 1;
		//vertex = vertex * camera;
		vertex.x /= vertex.z;
		vertex.y /= vertex.z;

	};

	modelMatrix *= glm::rotate(glm::radians(50.0f), vec3(0, 1, 0));


	auto mesh = Mesh();
	mesh.vertices = { {-0.1f,0.1f,0},{0.1f,0.1f,0},{-0.1f,-0.1f,0},{ 0.1f,-0.1f,0}};



	mesh.indexes = { 0,1,2,
					2, 1, 3};
	auto renderMesh = RenderMesh(mesh, vertexShader, fragmentShader);
	//renderMesh.fragmentShader = fragmentShader;
	vec3 p = vec3(0);
	std::function<void(vec3&)> copyFunction = fragmentShader;

	renderMesh.fragmentShader(p);

	DrawToScreen(renderMesh);

	 


	glClearColor(0, 1, 0, 1);
	while (true) {
		glfwPollEvents();
		
	

		
		glDrawPixels(600, 600, GL_RGB, GL_FLOAT, &pixels[0]);
		glfwSwapBuffers(window);
		glClear(GL_COLOR_BUFFER_BIT);

	}

}

*/