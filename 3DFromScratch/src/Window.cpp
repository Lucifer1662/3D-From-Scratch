#define GLEW_STATIC
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <vector>
#include <glm\glm.hpp>
#include <iostream>
#include <functional>
#include <glm\gtx\transform.hpp>
#include "Window.h"
#include <amp.h> 
using namespace concurrency;
using std::vector;
using glm::vec4;
using glm::vec3;
using glm::vec2;
using std::function;




namespace Window {
	vector<vec3> pixels;
	vec2ui windowSize = { 600,600 };
	rectui viewPort = { 0,0,windowSize.x,windowSize.y };
	rectui viewPortHalf = viewPort / 2u;
	GLFWwindow* window;


	




	
	/*
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
	
	
	*/



	



	static void WindowUpdated(uint width, uint height) {
		viewPort.z = windowSize.x = width;
		viewPort.w = windowSize.y = height;
		viewPortHalf = { 0,0,width / 2,height / 2 };
		pixels.resize(width * height);
	}

	void ChangeScreenSize(uint width, uint height)
	{
		glfwSetWindowSize(window, width, height);
		WindowUpdated(width, height);
	}
	

	void Create(int width, int height, const char* windowName, function<void()> startFunction, function<void()> updateFunction) {
		glewInit();
		glfwInit();

		window = glfwCreateWindow(width, height, windowName, NULL, NULL);
		glfwMakeContextCurrent(window);
		WindowUpdated(width,height);
		
		startFunction();

		uint sizeOfPixelsInBytes = sizeof(pixels[0]) * pixels.size();

		while (true) {
			glClear(GL_COLOR_BUFFER_BIT);
			memset(pixels.data(), 0, sizeOfPixelsInBytes);
			glfwPollEvents();
			updateFunction();

			glDrawPixels(viewPort.z, viewPort.w, GL_RGB, GL_FLOAT, &pixels[0]);
			glfwSwapBuffers(window);
		

		}

	}


}