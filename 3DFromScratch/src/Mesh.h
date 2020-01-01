#pragma once
#include <vector>
#include <glm\glm.hpp>
using std::vector;
using glm::vec3;
using uint = unsigned int;

struct Mesh {
	vector<vec3> vertices;
	vector<uint> indexes;
};
