#include "grid.h"
#include <vector>
#include <glm/glm.hpp>
#include <iostream>
#include "simulator.h"

using namespace glm;

void Grid::init(int size, double timestep, double viscosity) {
	/*Grid contains 1-cell border of 0-values on all sides*/
	timeStep = timestep;
	viscosity = viscosity;
	grid_size = size;
	cell_size = 2.0 / (grid_size + 2.);

	/******************************/
	/* GPU Process Initialization */
	/******************************/

	bindScreenVertices();
	bindSourceVertices();
	buildShaders();
	buildTextures();

	renderToScreen = fuelOutputTex;
}

/**********************/
/* GPU Initialization */
/**********************/

void Grid::buildShaders() {
	/* Shaders */
	defaultShader = new Shader("../src/shaders/defaultShader.vert", "../src/shaders/defaultShader.frag");
	borderShader = new Shader("../src/shaders/defaultShader.vert", "../src/shaders/borderShader.frag");
	fillShader = new Shader("../src/shaders/defaultShader.vert", "../src/shaders/circleShader.frag");

	//advectShader = new Shader("../src/shaders/defaultShader.vert", "../src/shaders/macCormack.frag");
	advectShader = new Shader("../src/shaders/defaultShader.vert", "../src/shaders/advectShader.frag");
	diffuseShader = new Shader("../src/shaders/defaultShader.vert", "../src/shaders/diffuseShader.frag");
	divergeShader = new Shader("../src/shaders/defaultShader.vert", "../src/shaders/divergence.frag");
	pressureShader = new Shader("../src/shaders/defaultShader.vert", "../src/shaders/pressure.frag");
	gradientShader = new Shader("../src/shaders/defaultShader.vert", "../src/shaders/gradientSubtraction.frag");

	directionalShader = new Shader("../src/shaders/defaultShader.vert", "../src/shaders/directionalShader.frag");
	splatShader = new Shader("../src/shaders/defaultShader.vert", "../src/shaders/splatShader.frag");
	fuelShader = new Shader("../src/shaders/defaultShader.vert", "../src/shaders/fuelShader.frag");
	colorShader = new Shader("../src/shaders/defaultShader.vert", "../src/shaders/colorShader.frag");
	buoyancyShader = new Shader("../src/shaders/defaultShader.vert", "../src/shaders/buoyancyShader.frag");
}
void Grid::buildTextures() {
	/* FBO, Texture Setup */
	FBO = new Framebuffer();
	std::vector<float> fbo_vel = std::vector<float>((grid_size+2)*(grid_size+2)*3, 0.f);
	velocityInputTex = FBO->createTexture(grid_size + 2, &fbo_vel[0]);
	velocityInputFBO = FBO->createFBO(velocityInputTex);

	advectionOutputTex = FBO->createTexture(grid_size + 2, &fbo_vel[0]);
	advectionOutputFBO = FBO->createFBO(advectionOutputTex);

	diffusionOutputTex = FBO->createTexture(grid_size + 2, &fbo_vel[0]);
	diffusionOutputFBO = FBO->createFBO(diffusionOutputTex);

	divergenceOutputTex = FBO->createTexture(grid_size + 2, &fbo_vel[0]);
	divergenceOutputFBO = FBO->createFBO(divergenceOutputTex);

	pressureOutputTex = FBO->createTexture(grid_size + 2, &fbo_vel[0]);
	pressureOutputFBO = FBO->createFBO(pressureOutputTex);

	temperatureTex = FBO->createTexture(grid_size + 2, &fbo_vel[0]);
	temperatureFBO = FBO->createFBO(temperatureTex);

	fuelOutputTex = FBO->createTexture(grid_size + 2, &fbo_vel[0]);
	fuelOutputFBO = FBO->createFBO(fuelOutputTex);

	colorOutputTex = FBO->createTexture(grid_size + 2, &fbo_vel[0]);
	colorOutputFBO = FBO->createFBO(colorOutputTex);

	bufferTex = FBO->createTexture(grid_size + 2, &fbo_vel[0]);
	bufferFBO = FBO->createFBO(bufferTex);

	buffer2Tex = FBO->createTexture(grid_size + 2, &fbo_vel[0]);
	buffer2FBO = FBO->createFBO(buffer2Tex);
}

/******************************/
/* INIT, COORDINATES, VISUALS */
/******************************/
void Grid::setVertices() {
	/*Grid contains 1-cell border of 0-values on all sides*/
	vertices = std::vector<vec3> (0);
	vertices.reserve((grid_size+3) * (grid_size+3));
	double inc = cell_size;
	double current_height = -1.f;
	for (int j = 0; j <= grid_size+2; j++) {
		double current_width = -1.f;
		for (int i = 0; i <= grid_size+2; i++) {
			vertices.push_back(vec3(current_width, current_height, 0.0f));
			current_width += inc;
		}
		current_height += inc;
	}
}
void Grid::setCentroids() {
	/*Grid contains 1-cell border of 0-values on all sides*/
	centroid_vecs = std::vector<vec3> ((grid_size+2) * (grid_size+2), vec3(0.f));
	double inc = cell_size;
	for (int j = 0; j < grid_size+2; j++) {
		for (int i = 0; i < grid_size+2; i++) {
			centroid_vecs[j*(grid_size+2) + i] = vec3((i + 0.5) * inc - 1.f, (j + 0.5) * inc - 1.f, 0.f);
		}
	}
}

int Grid::index (int x, int y) {
	return (y + 1)*(grid_size + 2) + x + 1;
}

void Grid::bindScreenVertices() {
	/* SCREEN VERTICES */
	float screen[] = {
		//Vertices       //Tex Coords
		-1.f, -1.f, 0.f,    0.f, 0.f,
		-1.f,  1.f, 0.f,    0.f, 1.f,
		 1.f,  1.f, 0.f,    1.f, 1.f,

		-1.f, -1.f, 0.f,    0.f, 0.f,
		 1.f, -1.f, 0.f,    1.f, 0.f,
		 1.f,  1.f, 0.f,    1.f, 1.f,
	};

	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);

	/* Bind VBO and set VBO data */
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 6 * 5 * sizeof(float), screen, GL_STATIC_DRAW);

	/* Bind VAO and set VAO configuration */
	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);

	/* Boundary Vertices */
	float border[] = {
		 //Vertices                                                  //Tex Coords
		-1.f + cell_size/2.f, -1.f + cell_size/2.f, 0.f,    0.f, 0.f, // LB
		-1.f + cell_size/2.f,  1.f - cell_size/2.f, 0.f,    0.f, 1.f, // LT
		 1.f - cell_size/2.f,  1.f - cell_size/2.f, 0.f,    1.f, 1.f, // RT
		 1.f - cell_size/2.f, -1.f + cell_size/2.f, 0.f,    1.f, 0.f, // RB
		-1.f + cell_size/2.f, -1.f + cell_size/2.f, 0.f,    0.f, 0.f, // LB
	};

	glGenBuffers(1, &bVBO);
	glGenVertexArrays(1, &bVAO);

	/* Bind VBO and set VBO data */
	glBindBuffer(GL_ARRAY_BUFFER, bVBO);
	glBufferData(GL_ARRAY_BUFFER, 5 * 5 * sizeof(float), border, GL_STATIC_DRAW);

	/* Bind VAO and set VAO configuration */
	glBindVertexArray(bVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);


	/* Unbind */
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Grid::bindSourceVertices() {
	/* Boundary Vertices */
	float source[] = {
		//Vertices       //Tex Coords
		-0.1f, -0.9f, 0.f,    0.5f, 0.05f,
		-0.1f, -0.8f, 0.f,    0.5f, 0.1f,
		 0.1f, -0.8f, 0.f,    0.5f, 0.05f,

		 0.1f, -0.8f, 0.f,    0.5f, 0.1f,
		 0.1f, -0.9f, 0.f,    0.5f, 0.05f,
		-0.1f, -0.9f, 0.f,    0.5f, 0.05f
	};

	glGenBuffers(1, &sVBO);
	glGenVertexArrays(1, &sVAO);

	/* Bind VBO and set VBO data */
	glBindBuffer(GL_ARRAY_BUFFER, sVBO);
	glBufferData(GL_ARRAY_BUFFER, 6 * 5 * sizeof(float), source, GL_STATIC_DRAW);

	/* Bind VAO and set VAO configuration */
	glBindVertexArray(sVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);


	/* Unbind */
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

/**************/
/* Simulation */
/**************/

/* GPU SIMULATION */
void Grid::stepOnce(int iterations) {

	// Note: ALL CELL SIZES ARE WRITTEN IN GL SPACE (-1 to 1)
	// Multiply by .5 in shaders to convert to texture space

	/* Setup */
	glViewport(0, 0, grid_size + 2, grid_size + 2);

	/* Advect */
	glBindVertexArray(VAO);
	glBindFramebuffer(GL_FRAMEBUFFER, advectionOutputFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	advectShader->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, velocityInputTex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, velocityInputTex);
	defaultShader->setInt("inVelocity", 0);
	defaultShader->setInt("inQuantity", 1);
	advectShader->setFloat("cellSize", cell_size);
	advectShader->setFloat("timeStep", timeStep);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	/* Write to Velocity Texture */
	glBindFramebuffer(GL_FRAMEBUFFER, velocityInputFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	defaultShader->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, advectionOutputTex);
	defaultShader->setInt("input_texture", 0);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	/* Diffusion Loop */
	glBindVertexArray(VAO);
	//Copy Vel to buffer
	defaultShader->use();
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, velocityInputTex);
	defaultShader->setInt("input_texture", 0);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	diffuseShader->use();
	diffuseShader->setFloat("viscosity", viscosity);
	diffuseShader->setFloat("cellSize", cell_size);
	diffuseShader->setFloat("timeStep", timeStep);
	for (int i = 0; i < iterations; i++) {
		/* Buffer -> Diffusion */
		glBindFramebuffer(GL_FRAMEBUFFER, diffusionOutputFBO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, bufferTex);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		/* Diffusion -> Buffer */
		glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffusionOutputTex);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	//Copy to Velocity
	defaultShader->use();
	glBindFramebuffer(GL_FRAMEBUFFER, velocityInputFBO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferTex);
	defaultShader->setInt("input_texture", 0);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	/* Unbind */
	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Grid::extForces(float time) {
	glBindVertexArray(VAO);

	/* Copy Old Velocity to Buffer */
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	defaultShader->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, velocityInputTex);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	/* Gaussian Splat*/
	glBindFramebuffer(GL_FRAMEBUFFER, velocityInputFBO);
	splatShader->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferTex);
	vec4 center = vec4(0.f, 0.f, 0.f, 0.f);
	splatShader->setVec4("center", center);
	splatShader->setFloat("radius", 0.2f);
	vec4 force = vec4(0.f, 0.f, 0.f, 0.f);
	force.x = (rand() % 10) - 5;
	force.y = (rand() % 5);
	force /= 1000.f;
	force /= (int) (rand() % 100);
	splatShader->setVec4("force", force);
	splatShader->setFloat("timeStep", timeStep);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	/* Buoyancy */
	//Copy to buffer
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	defaultShader->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, velocityInputTex);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	//Buoyancy
	glBindFramebuffer(GL_FRAMEBUFFER, velocityInputFBO);
	buoyancyShader->use();
	buoyancyShader->setFloat("speed", .02f);
	buoyancyShader->setFloat("ambient", 0.0f);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferFBO);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, temperatureFBO);
	buoyancyShader->setInt("inVelocity", 0);
	buoyancyShader->setInt("inTemperature", 1);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	/* Directional */
	//Copy to buffer
	glBindVertexArray(VAO);
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	defaultShader->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, velocityInputTex);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	//Directional
	glBindFramebuffer(GL_FRAMEBUFFER, velocityInputFBO);
	directionalShader->use();
	directionalShader->setFloat("time", time + ((rand() % 10)/.5f));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferFBO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	/* Unbind */
	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Grid::projectGPU(int iterations) {
	/* Calculate Divergence */
	glBindVertexArray(VAO);
	glBindFramebuffer(GL_FRAMEBUFFER, divergenceOutputFBO);
	divergeShader->use();
	divergeShader->setFloat("cellSize", cell_size);
	divergeShader->setFloat("timeStep", timeStep);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, velocityInputTex);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	/* Pressure Loop */
	glBindFramebuffer(GL_FRAMEBUFFER, pressureOutputFBO);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	pressureShader->use();
	pressureShader->setFloat("cellSize", cell_size);
	pressureShader->setFloat("timeStep", timeStep);

	for (int i = 0; i < iterations; i++) {
		/* Copy Pressure to Buffer */
		glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, pressureOutputTex);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, divergenceOutputTex);
		pressureShader->setInt("inPressure", 0);
		pressureShader->setInt("divergences", 1);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		/* Write to Pressure */
		glBindFramebuffer(GL_FRAMEBUFFER, pressureOutputFBO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, bufferTex);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, divergenceOutputTex);
		pressureShader->setInt("inPressure", 0);
		pressureShader->setInt("divergences", 1);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	/* Gradient Subtraction */
	//Copy Velocity to Buffer
	glBindVertexArray(VAO);
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	defaultShader->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, velocityInputTex);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	//Subtract
	glBindFramebuffer(GL_FRAMEBUFFER, velocityInputFBO);
	gradientShader->use();
	gradientShader->setFloat("cellSize", cell_size);
	gradientShader->setFloat("timeStep", timeStep);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferTex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, pressureOutputTex);
	gradientShader->setInt("inVelocity", 0);
	gradientShader->setInt("inPressure", 1);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	drawBoundary(0);
	drawBoundary(1);

	/* Unbind */
	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Grid::moveDye(float time) {
	glBindVertexArray(VAO);

	/* Copy Old Fuel Distribution to Buffer */
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	defaultShader->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fuelOutputTex);
	defaultShader->setInt("input_texture", 0);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	/* Fill Source */
	glBindFramebuffer(GL_FRAMEBUFFER, fuelOutputFBO);
	fillShader->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferTex);
	defaultShader->setInt("input_texture", 0);
	vec4 center = vec4(0.f, -0.7f, 0.f, 0.f);
	fillShader->setVec4("center", center);
	fillShader->setFloat("radius", 0.1f);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	/* Copy Old Fuel Distribution to Buffer */
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	defaultShader->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fuelOutputTex);
	defaultShader->setInt("input_texture", 0);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	/* Advect Fuel */
	glBindFramebuffer(GL_FRAMEBUFFER, fuelOutputFBO);
	advectShader->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, velocityInputTex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bufferTex);
	advectShader->setInt("inVelocity", 0);
	advectShader->setInt("inQuantity", 1);
	advectShader->setFloat("cellSize", cell_size);
	advectShader->setFloat("timeStep", timeStep);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	/* Copy Old Fuel Distribution to Buffer */
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	defaultShader->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fuelOutputTex);
	defaultShader->setInt("input_texture", 0);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	/* Consume Fuel */
	glBindFramebuffer(GL_FRAMEBUFFER, fuelOutputFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	fuelShader->use();
	fuelShader->setFloat("timeStep", timeStep);
	fuelShader->setFloat("rate", 0.5f);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferTex);
	defaultShader->setInt("input_texture", 0);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	/* Diffusion Loop */
	glBindVertexArray(VAO);
	diffuseShader->use();
	diffuseShader->setFloat("viscosity", viscosity);
	diffuseShader->setFloat("cellSize", cell_size);
	diffuseShader->setFloat("timeStep", timeStep);
	for (int i = 0; i < 15; i++) {
		/* Buffer -> Diffusion */
		glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, fuelOutputTex);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		/* Diffusion -> Buffer */
		glBindFramebuffer(GL_FRAMEBUFFER, fuelOutputFBO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, bufferTex);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	/* Use new fuel values to update temperature */
	defaultShader->use();
	glBindFramebuffer(GL_FRAMEBUFFER, temperatureFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fuelOutputTex);
	defaultShader->setInt("input_texture", 0);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	/* Unbind */
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindVertexArray(0);
}

void Grid::drawBoundary(int type) {
	/* Copy to Buffer */
	glBindVertexArray(VAO);
	defaultShader->use();
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);

	switch (type) {
	case 0: //VELOCITY
		glBindTexture(GL_TEXTURE_2D, velocityInputTex);
		break;
	case 1: //PRESSURE
		glBindTexture(GL_TEXTURE_2D, pressureOutputTex);
		break;
	}
	defaultShader->setInt("input_texture", 0);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	/* Draw Boundary */
	vec4 top, bottom, left, right;
	top = vec4(0.f, -cell_size / 2.f, 0.f, 0.f);
	bottom = vec4(0.f, cell_size / 2.f, 0.f, 0.f);
	left = vec4(cell_size / 2.f, 0.f, 0.f, 0.f);
	right = vec4(-cell_size / 2.f, 0.f, 0.f, 0.f);

	glBindVertexArray(bVAO);
	borderShader->use();
	borderShader->setFloat("cellSize", cell_size);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferTex);
	defaultShader->setInt("input_texture", 0);
	switch (type) {
	case 0: //VELOCITY
		glBindFramebuffer(GL_FRAMEBUFFER, velocityInputFBO);
		borderShader->setFloat("scalar", -1.f);
		break;
	case 1: //PRESSURE
		glBindFramebuffer(GL_FRAMEBUFFER, pressureOutputFBO);
		borderShader->setFloat("scalar", 1.f);
		break;
	}

	borderShader->setVec4("offset", left);
	glDrawArrays(GL_LINES, 0, 2);
	borderShader->setVec4("offset", top);
	glDrawArrays(GL_LINES, 1, 2);
	borderShader->setVec4("offset", right);
	glDrawArrays(GL_LINES, 2, 2);
	borderShader->setVec4("offset", bottom);
	glDrawArrays(GL_LINES, 3, 3);

	/* Unbind */
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindVertexArray(0);
}

/* CPU COMPUTATIONS */
void Grid::calculateVelocity(float time) {
	//Test function
	for (unsigned int i = 0; i < velocities.size(); i++) {
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
	int box_x = floor((position.x + 1.0)/cell_size) - 1;
	int box_y = floor((position.y + 1.0)/cell_size) - 1;
	box_x = clamp(box_x, 0, grid_size - 1);
	box_y = clamp(box_y, 0, grid_size - 1);

	int horizontal_neighbor, vertical_neighbor;
	double t_x, t_y;

	/* HORIZONTAL NEIGHBOR TEST */
	float test_left = distance(centroid_vecs[index(box_x-1, box_y)], position);
	float test_right = distance(centroid_vecs[index(box_x+1, box_y)], position);
	if (test_left < test_right) {
		horizontal_neighbor = box_x - 1;
	} else {
		horizontal_neighbor = box_x + 1;
	}
	t_x = abs(position.x - centroid_vecs[index(box_x, box_y)].x)/cell_size;
	t_x = clamp(t_x, 0., 1.);

	/* VERTICAL NEIGHBOR TEST */
	float test_top = distance(centroid_vecs[index(box_x, box_y + 1)], position);
	float test_bottom = distance(centroid_vecs[index(box_x, box_y - 1)], position);
	if (test_top < test_bottom) {
		vertical_neighbor = box_y + 1;
	} else {
		vertical_neighbor = box_y - 1;
	}
	t_y = abs(position.y - centroid_vecs[index(box_x, box_y)].y)/cell_size;
	t_y = clamp(t_y, 0., 1.);

	// TODO: make this more general to include both velocities and pressures, etc.
	dvec3 first_ho = lerp(t_x, velocities[index(horizontal_neighbor, box_y)],
								velocities[index(box_x, box_y)]);
	dvec3 second_ho = lerp(t_x, velocities[index(horizontal_neighbor, vertical_neighbor)],
								velocities[index(box_x, vertical_neighbor)]);
	dvec3 vertical = lerp(t_y, second_ho, first_ho);
	return vertical;
}

void Grid::calculateAdvection() {
	old_velocities = velocities;
	for (int j = 0; j < grid_size; ++j) {
		for (int i = 0; i < grid_size; ++i) {
			int n = index(i, j);
			dvec3 currPos = centroid_vecs[n];
			dvec3 currVelocity = velocities[n] * timeStep;
			dvec3 prevPos = currPos - currVelocity;
			velocities[n] = nearestBilerp(prevPos);
		}
	}
}

void Grid::jacobiStepDiffuse(int i, int j) {
	//TODO
	int n = index(i, j);
	dvec3 alpha = (velocities[n] - old_velocities[n]) * (velocities[n] - old_velocities[n]) / timeStep / viscosity;
	dvec3 beta = dvec3(1./(4. + alpha.x), 1./(4. + alpha.y), 0);
	dvec3 L = velocities[n - 1];
	dvec3 R = velocities[n + 1];
	dvec3 T = velocities[index(i, j+1)];
	dvec3 B = velocities[index(i, j-1)];
	dvec3 self = velocities[n];
	old_velocities[n] = velocities[n];
	velocities[n] = (L + R + B + T + alpha * self) * beta;
}

void Grid::calculateDiffusion(int iterations) {
	for (int j = 0; j < grid_size; j++) {
		for (int i = 0; i < grid_size; i++) {
			for (int n = 0; n < iterations; n++) {
				jacobiStepDiffuse(i, j);
			}
		}
	}
}

void Grid::calculateDivergence() {
	divergences = std::vector<double> (velocities.size(), 0.); //new holder texture with divergences
	for (int j = 0; j < grid_size; ++j) {
		for (int i = 0; i < grid_size; ++i) {
			int n = index(i, j);
			dvec3 L = velocities[n - 1];
			dvec3 R = velocities[n + 1];
			dvec3 T = velocities[index(i, j + 1)];
			dvec3 B = velocities[index(i, j - 1)];
			double div = ((R.x - L.x) + (T.y - B.y))/2.;
			//do we need to calculate 3D with the front and back coordinates too?
			divergences[n] = div;
		}
	}
}

void Grid::jacobiStepPressure(int i, int j) {
	int n = index(i, j);
	float alpha = -pow(pressures[n] - old_pressures[n], 2);
	float beta = 1. / 4.;
	float L = pressures[n - 1];
	float R = pressures[n + 1];
	float T = pressures[index(i, j + 1)];
	float B = pressures[index(i, j - 1)];
	float b = divergences[n];
	old_pressures[n] = pressures[n];
	pressures[n] = (L + R + B + T + alpha * b) * beta;
}

void Grid::gradientSubtraction() {
	//std::vector<dvec3> newVel;
	for (int j = 0; j < grid_size; ++j) {
		for (int i = 0; i < grid_size; ++i) {
			int n = index(i, j);
			float L = pressures[n - 1];
			float R = pressures[n + 1];
			float T = pressures[index(i, j + 1)];
			float B = pressures[index(i, j - 1)];
			dvec3 v = velocities[n] - (dvec3((R - L) / 2.0, (T - B) / 2.0, 0.) * .5);
			velocities[n] = v;
			//newVel.push_back(v);
		}
	}
}

void Grid::project(int iterations) {
	//calculateDivergence();
	divergences = std::vector<double>(old_pressures.size(), 0.f);
	for (int j = 0; j < grid_size; j++) {
		for (int i = 0; i < grid_size; i++) {
			for (int n = 0; n < iterations; n++) {
				jacobiStepPressure(i, j);
			}
		}
	}
	gradientSubtraction();
}

void Grid::boundaryConditions() {
	//Bottom and Top Border
	for (int i = 0; i < grid_size + 2; i++) {
		pressures[i] = pressures[grid_size + 2 + i];
		pressures[(grid_size + 1)*(grid_size + 2) + i] = pressures[grid_size*(grid_size + 2) + i];
	}
	//Side Borders
	for (int j = 1; j < grid_size + 1; j++) {
		pressures[j*(grid_size + 2)] = pressures[j * (grid_size + 2) + 1];
		pressures[j*(grid_size + 2) + (grid_size + 1)] = pressures[j*(grid_size + 2) + grid_size];
	}
}