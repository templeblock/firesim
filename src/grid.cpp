#include "grid.h"

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
