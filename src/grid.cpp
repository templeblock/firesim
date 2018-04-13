#include "grid.h"
#include <vector>
#include <glm/glm.hpp>
#include <iostream>

using namespace glm;

void Grid::init(int size) {
	grid_size = size;
	cell_size = 2.0 / (double)grid_size;

	velocities = std::vector<vec3>(grid_size * grid_size, vec3(0.f, 1.f, 0.f));
	pressures = std::vector<float>(grid_size * grid_size, 0.0);

	setVertices();
	setCentroids();
}

void Grid::setVertices()
{
	vertices = std::vector<vec3> (0);
	vertices.reserve((grid_size+1) * (grid_size+1));
	double inc = cell_size;
	double current_height = -1.f;
	for (int j = 0; j <= grid_size; j++) {
		double current_width = -1.f;
		for (int i = 0; i <= grid_size; i++) {
			vertices.push_back(vec3(current_width, current_height, 0.0f));
			current_width += inc;
		}
		current_height += inc;
	}
}

void Grid::setCentroids() {
	centroid_vecs = std::vector<vec3> (grid_size * grid_size, vec3(0.f));
	double inc = cell_size;
	for (int j = 0; j < grid_size; j++) {
		for (int i = 0; i < grid_size; i++) {
			centroid_vecs[j*grid_size + i] = vec3((i + 0.5) * inc - 1.0, (j + 0.5) * inc - 1.0, 0.f);
		}
	}
}