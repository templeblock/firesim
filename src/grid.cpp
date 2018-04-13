#include "grid.h"
#include <vector>
#include <glm/glm.hpp>
using namespace glm;

void Grid::init() {

	float newVertices[] = {
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
	grid_size = 3;
}

int Grid::getVertexArraySize()
{
	//return grid_size * grid_size * 3;
	return 27;
}

std::vector<vec3> Grid::getVertices()
{
	int gridSize = 3;
	std::vector<vec3> vert;
	double inc = 2.0 / (double) gridSize;
	for (int i = 0; i <= gridSize; i++) {
		for (int j = 0; j <= gridSize; j++) {
			vert.push_back(vec3(i * inc - 1.0, j * inc - 1.0, 0.0));
		}
	}
	return vert;
}

std::vector<vec3> Grid::getCentroids() {
	int gridSize = 3;
	std::vector<vec3> cent;
	double inc = 2.0 / (double)gridSize;
	for (int i = 0; i < gridSize; i++) {
		for (int j = 0; j < gridSize; j++) {
			cent.push_back(vec3((i + 0.5) * inc - 1.0, (j + 0.5) * inc - 1.0, 0.0));
		}
	}
	return cent;
}
