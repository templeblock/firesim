#ifndef GRID_H
#define GRID_H
#include <vector>
#include <glm/glm.hpp>
using namespace glm;

#include "framebuffer.h"
#include "shader.h"

class Grid {

public:

	/*Init*/
	Grid() {}
	void init(int size, double timestep, double viscosity);

	/* Init, Dimensions, Locations */
	void setVertices();
	void setCentroids();

	int index (int x, int j);

	void bindScreenVertices();

	void bindSourceVertices();

	void stepOnce(int iterations);

	void extForces(float time);

	void moveDye(float time);

	/* Simulation */
	void calculateVelocity(float time);

	vec3 nearestBilerp(vec3 position);
	void calculateAdvection();

	void calculateDiffusion(int iterations);
	void jacobiStepDiffuse(int i, int j);
	void project(int iterations);
	void jacobiStepPressure(int i, int j);

	void calculateDivergence();
	void gradientSubtraction();

	void boundaryConditions();

	/* Simulation Settings */
	int grid_size;
	float cell_size;
	double timeStep;
	/* Fluid Settings */
	double viscosity;

	/* Simulation Data */
	GLuint renderToScreen;

	/* CPU Computation Data */
	std::vector<vec3> vertices;
	std::vector<vec3> centroid_vecs;

	std::vector<dvec3> velocities;
	std::vector<dvec3> old_velocities;
	std::vector<double> divergences;
	std::vector<float> pressures;
	std::vector<float> old_pressures;

private:
	Framebuffer *FBO;

	//Shaders
	Shader *defaultShader, *bVelShader;
	Shader *advectShader, *diffuseShader, *divergeShader, *pressureShader, *gradientShader;
	Shader *directionalShader, *circleShader, *buoyancyShader;

	GLuint VBO, VAO; //Screen vertices
	GLuint bVBO, bVAO; //Border vertices
	GLuint sVBO, sVAO; //Source vertices

	//Vector value textures - RGB correspond to XYZ
	GLuint centroidsFBO, velocityInputFBO, advectionOutputFBO, diffusionOutputFBO;
	GLuint centroidsTex, velocityInputTex, advectionOutputTex, diffusionOutputTex;
	GLuint dyeOutputFBO, dyeOutputTex;

	GLuint buoyancyOutputFBO, temperatureFBO;
	GLuint buoyancyOutputTex, temperatureTex;

	//Scalar textures - RG correspond to divergence, pressure
	GLuint scalarsOutputFBO, scalarsOutputTex;

	//Buffer textures for extra R/W
	GLuint bufferFBO, bufferTex, buffer2FBO, buffer2Tex;
};

#endif