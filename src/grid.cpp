#include "grid.h"
#include <vector>
#include <glm/glm.hpp>
#include <iostream>
#include "simulator.h"

using namespace glm;

void Grid::init(int size) {
	grid_size = size;
	cell_size = 2.0 / (double)grid_size;

	velocities = std::vector<dvec3>(grid_size * grid_size, dvec3(0.f, 1.f, 0.f));
	//Randomly generate vector field
	for (int i = 0; i < velocities.size(); i++) {
		float random_x = rand() % 20;
		float random_y = rand() % 20;
		random_x = (random_x - 10) / 10;
		random_y = (random_y - 10) / 10;
		velocities[i] = dvec3(random_x, random_y, 0.f);
		if (length(velocities[i]) != 0) {
			velocities[i] = normalize(velocities[i]);
		}
	}
	pressures = std::vector<float>(grid_size * grid_size, 0.0);

	setVertices();
	setCentroids();
}

/******************************/
/* INIT, COORDINATES, VISUALS */
/******************************/

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

/**************/
/* Simulation */
/**************/

void Grid::calculateVelocity(float time) {
	//Test function
	for (int i = 0; i < velocities.size(); i++) {
		velocities[i].x = sin(time);
		velocities[i].y = cos(time);
	}
}

vec3 lerp(float t, vec3 a, vec3 b) {
	return t * b + (1.f - t) * a;
	//return a + ((b - a) * t);
}

vec3 Grid::nearestBilerp(vec3 position) {
	// Find index of cell that holds position
	int box_x = floor((position.x + 1.0)/cell_size);
	int box_y = floor((position.y + 1.0)/cell_size);
	box_x = clamp(box_x, 0, grid_size - 1);
	box_y = clamp(box_y, 0, grid_size - 1);

	int horizontal_neighbor, vertical_neighbor;
	float t_x, t_y;

	// test against centroids of horizontal neighboring cells
	if (box_x == 0) {
		horizontal_neighbor = box_x + 1;
		t_x = 0;
	} else if (box_x == grid_size - 1) {
		horizontal_neighbor = box_x - 1;
		t_x = 0;
	} else {
		float test_left = distance(centroid_vecs[box_y * grid_size + (box_x - 1)], position);
		float test_right = distance(centroid_vecs[box_y * grid_size + (box_x + 1)], position);
		if (test_left < test_right) {
			horizontal_neighbor = box_x - 1;
		} else {
			horizontal_neighbor = box_x + 1;
		}
		t_x = abs(position.x - centroid_vecs[box_y * grid_size + box_x].x);
		t_x /= cell_size;
	}
	// test against centroids of vertical neighboring cells
	if (box_y == 0) {
		vertical_neighbor = box_y + 1;
		t_y = 0;
	} else if (box_y == grid_size - 1) {
		vertical_neighbor = box_y - 1;
		t_y = 0;
	} else {
		float test_top = distance(centroid_vecs[(box_y + 1) * grid_size + box_x], position);
		float test_bottom = distance(centroid_vecs[(box_y - 1) * grid_size + box_x], position);
		if (test_top < test_bottom) {
			vertical_neighbor = box_y + 1;
		} else {
			vertical_neighbor = box_y - 1;
		}
		t_y = abs(position.y - centroid_vecs[box_y * grid_size + box_x].y);
		t_y /= cell_size;
	}

	// TODO: make this more general to include both velocities and pressures, etc.
	dvec3 first_ho = lerp(t_x, velocities[(box_y * grid_size) + horizontal_neighbor],
														velocities[(box_y * grid_size) + box_x]);
	dvec3 second_ho = lerp(t_x, velocities[(vertical_neighbor * grid_size) + horizontal_neighbor],
														velocities[(vertical_neighbor * grid_size) + box_x]);
	dvec3 vertical = lerp(t_y, second_ho, first_ho);
	return vertical;
}

void Grid::calculateAdvection(double deltaTime) {
	for (int j = 0; j < grid_size; ++j) {
		for (int i = 0; i < grid_size; ++i) {
			dvec3 currPos = centroid_vecs[j * grid_size + i];
			dvec3 currVelocity = velocities[j * grid_size + i] * .2; //TODO change scalar to timestep; check that velocity magnitude is at least cell_size
			dvec3 prevPos = currPos - currVelocity;
			velocities[j * grid_size + i] = nearestBilerp(prevPos);
		}
	}
}