#pragma once
#include <Mesh.h>
#include <functional>

using std::function;
using glm::vec4;

struct RenderMesh {
	const Mesh& mesh;
	function<void(vec4&)>& vertexShader;
	function<void(vec3&)>& fragmentShader;
	
	RenderMesh(const Mesh& mesh, function<void(vec4&)>& vertexShader, function<void(vec3&)>& fragmentShader) :
		mesh(mesh), vertexShader(vertexShader), fragmentShader(fragmentShader) {}
};