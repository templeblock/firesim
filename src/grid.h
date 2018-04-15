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
	void init(int size, double timestep, double viscosity);

	/* Init, Dimensions, Locations */
	void setVertices();
	void setCentroids();

	int index (int x, int j);

	/* Simulation */
	void calculateVelocity(float time);

	vec3 nearestBilerp(vec3 position);
	void calculateAdvection();

	void calculateDiffusion(int iterations);
	void jacobiStepDiffuse(int i, int j);

	void calculatePressure(int iterations);

	int grid_size;
	float cell_size;
	std::vector<vec3> vertices;
	std::vector<vec3> centroid_vecs;

	double timeStep;
	double viscosity;
	std::vector<dvec3> velocities;
	std::vector<dvec3> old_velocities;
	std::vector<float> pressures;
	std::vector<float> old_pressures;

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