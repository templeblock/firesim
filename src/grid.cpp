#include "grid.h"
#include <vector>
#include <glm/glm.hpp>
using namespace glm;

void Grid::init() {

	grid_size = 3;

	float newVertices[27] = {
		-1.f, -1.f, 0.5f,
		0.f, -1.f, 0.5f,
		1.f, -1.f, 0.5f,
		-1.f, 0.f, 0.5f,
		0.f, 0.f, 0.5f,
		1.f, 0.f, 0.5f,
		-1.f, 1.f, 0.5f,
		0.f, 1.f, 0.5f,
		1.f, 1.f, 0.5f
	};
	vertices = newVertices;

	velocities = std::vector<vec2>(grid_size * grid_size, vec2(0.0));
	pressures = std::vector<float>(grid_size * grid_size, 0.0);

}

int Grid::getVertexArraySize()
{
	//return grid_size * grid_size * 3;
	return 27;
}

std::vector<vec3> Grid::getVertices()
{
	std::vector<vec3> vert;
	double inc = 2.0 / (double)grid_size;
	for (int i = 0; i <= grid_size; i++) {
		for (int j = 0; j <= grid_size; j++) {
			vert.push_back(vec3(i * inc - 1.0, j * inc - 1.0, 0.0));
		}
	}
	return vert;
}

std::vector<vec3> Grid::getCentroids() {
	std::vector<vec3> cent;
	double inc = 2.0 / (double)grid_size;
	for (int i = 0; i < grid_size; i++) {
		for (int j = 0; j < grid_size; j++) {
			cent.push_back(vec3((i + 0.5) * inc - 1.0, (j + 0.5) * inc - 1.0, 0.0));
		}
	}
	return cent;
}
