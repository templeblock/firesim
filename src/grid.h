#ifndef GRID_H
#define GRID_H
#include <vector>

#include <vector>
#include <glm/glm.hpp>
using namespace glm;

class Grid {

public:

	/*Init*/
	Grid() {}
	void init(int size);

	/* Init, Dimensions, Locations */
	void setVertices();
	void setCentroids();

	/* Simulation */
	void calculateVelocity(float deltaTime);

	int grid_size;
	float cell_size;
	std::vector<vec3> vertices;
	std::vector<vec3> centroid_vecs;
	std::vector<vec3> arrows;

	std::vector<vec3> velocities;
	std::vector<float> pressures;

	/*Simulation*/
	//calculate advection func
	//calculate pressure func
	//calculate diffusion func
	//calculate external forces func
	//write to pixel buffer func (ink)

	/*Fluid Property Constants*/
	//Density Constant
	//Viscosity Constant

	/*Simulation State Variables*/
	//Velocity Texture
	//Pressure Texture
	//Pixel Buffer Array Texture

private:

	//who the fuck

};

#endif