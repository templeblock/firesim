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
	void init();

	int getVertexArraySize();
	std::vector<vec3> getVertices();
	std::vector<vec3> getCentroids();
	std::vector<vec3> getScalarEndpoints();

	int grid_size;
	std::vector<vec3> velocities;
	std::vector<float> pressures;
	float *vertices;

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