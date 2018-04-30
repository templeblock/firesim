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
	defaultShader = new Shader("../src/shaders/defaultShader.vert", "../src/shaders/default3DShader.frag");
	borderShader = new Shader("../src/shaders/defaultShader.vert", "../src/shaders/borderShader.frag");
	fillShader = new Shader("../src/shaders/defaultShader.vert", "../src/shaders/circleShader.frag");

	advectShader = new Shader("../src/shaders/defaultShader.vert", "../src/shaders/macCormack.frag");
	//advectShader = new Shader("../src/shaders/defaultShader.vert", "../src/shaders/advectShader.frag");
	diffuseShader = new Shader("../src/shaders/defaultShader.vert", "../src/shaders/diffuseShader.frag");
	divergeShader = new Shader("../src/shaders/defaultShader.vert", "../src/shaders/divergence.frag");
	pressureShader = new Shader("../src/shaders/defaultShader.vert", "../src/shaders/pressure.frag");
	gradientShader = new Shader("../src/shaders/defaultShader.vert", "../src/shaders/gradientSubtraction.frag");

	directionalShader = new Shader("../src/shaders/defaultShader.vert", "../src/shaders/directionalShader.frag");
	splatShader = new Shader("../src/shaders/defaultShader.vert", "../src/shaders/splatShader.frag");
	fuelShader = new Shader("../src/shaders/defaultShader.vert", "../src/shaders/fuelShader.frag");
	buoyancyShader = new Shader("../src/shaders/defaultShader.vert", "../src/shaders/buoyancyShader.frag");
}

void Grid::buildTextures() {
	/* FBO, Texture Setup */
	FBO = new Framebuffer();
	std::vector<float> fbo_vel = std::vector<float>((grid_size+2)*(grid_size+2)*(grid_size+2)*3, 0.f);

	velocityInputTex = FBO->create3DTexture(grid_size + 2, &fbo_vel[0]);
	advectionOutputTex = FBO->create3DTexture(grid_size + 2, &fbo_vel[0]);
	diffusionOutputTex = FBO->create3DTexture(grid_size + 2, &fbo_vel[0]);

	divergenceOutputTex = FBO->create3DTexture(grid_size + 2, &fbo_vel[0]);
	pressureOutputTex = FBO->create3DTexture(grid_size + 2, &fbo_vel[0]);

	temperatureTex = FBO->create3DTexture(grid_size + 2, &fbo_vel[0]);
	fuelOutputTex = FBO->create3DTexture(grid_size + 2, &fbo_vel[0]);

	bufferTex = FBO->create3DTexture(grid_size + 2, &fbo_vel[0]);
	buffer2Tex = FBO->create3DTexture(grid_size + 2, &fbo_vel[0]);

	writeFBO = FBO->create3DFBO(bufferTex);
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
	glBindFramebuffer(GL_FRAMEBUFFER, writeFBO);
	glBindVertexArray(VAO);

	/* Advect */
	advectShader->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, velocityInputTex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_3D, velocityInputTex);
	advectShader->setInt("inVelocity", 0);
	advectShader->setInt("inQuantity", 1);
	advectShader->setFloat("cellSize", cell_size);
	advectShader->setFloat("timeStep", timeStep);
	for (int i = 0; i < grid_size + 2; i++) {
		advectShader->setFloat("slice", i);
		FBO->switchLayer(advectionOutputTex, i);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	/* Write to Velocity Texture */
	defaultShader->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, advectionOutputTex);
	defaultShader->setFloat("cellSize", cell_size);
	for (int i = 0; i < grid_size + 2; i++) {
		defaultShader->setFloat("slice", i);
		FBO->switchLayer(velocityInputTex, i);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	/* Diffusion Loop */
	//Copy Vel to buffer
	defaultShader->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, velocityInputTex);
	defaultShader->setInt("input_texture", 0);
	for (int i = 0; i < grid_size + 2; i++) {
		defaultShader->setFloat("slice", i);
		FBO->switchLayer(bufferTex, i);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	diffuseShader->use();
	diffuseShader->setFloat("viscosity", viscosity);
	diffuseShader->setFloat("cellSize", cell_size);
	diffuseShader->setFloat("timeStep", timeStep);
	for (int i = 0; i < iterations; i++) {
		/* Buffer -> Diffusion */
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, bufferTex);
		for (int i = 0; i < grid_size + 2; i++) {
			diffuseShader->setFloat("slice", i);
			FBO->switchLayer(diffusionOutputTex, i);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		/* Diffusion -> Buffer */
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, diffusionOutputTex);
		for (int i = 0; i < grid_size + 2; i++) {
			diffuseShader->setFloat("slice", i);
			FBO->switchLayer(bufferTex, i);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
	}

	////Copy to Velocity
	//defaultShader->use();
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_3D, bufferTex);
	//defaultShader->setInt("input_texture", 0);
	//for (int i = 0; i < grid_size + 2; i++) {
	//	defaultShader->setFloat("slice", i);
	//	FBO->switchLayer(velocityInputTex, i);
	//	glDrawArrays(GL_TRIANGLES, 0, 6);
	//}

	/* Unbind */
	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_3D, 0);
}

void Grid::extForces(float time) {
	glBindVertexArray(VAO);
	glBindFramebuffer(GL_FRAMEBUFFER, writeFBO);

	///* Copy Old Velocity to Buffer */
	//defaultShader->use();
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_3D, velocityInputTex);
	//for (int i = 0; i < grid_size + 2; i++) {
	//	defaultShader->setFloat("slice", i);
	//	FBO->switchLayer(bufferTex, i);
	//	glDrawArrays(GL_TRIANGLES, 0, 6);
	//}


	/* Gaussian Splat*/
	splatShader->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, bufferTex);
	vec4 center = vec4(0.f, -.3f, 0.f, 0.f);
	splatShader->setFloat("cellSize", cell_size);
	splatShader->setVec4("center", center);
	splatShader->setFloat("radius", 0.06f);
	vec4 force = vec4(0.f, 0.f, 0.f, 0.f);
	force.x = (rand() % 10) - 5;
	force.y = (rand() % 5);
	force.z = (rand() % 5);
	force /= 10000.f;
	force /= (int) (rand() % 100);
	splatShader->setVec4("force", force);
	splatShader->setFloat("timeStep", timeStep);
	for (int i = 0; i < grid_size + 2; i++) {
		splatShader->setFloat("slice", i);
		FBO->switchLayer(velocityInputTex, i);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	/* Buoyancy */
	//Copy to buffer
	defaultShader->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, velocityInputTex);
	for (int i = 0; i < grid_size + 2; i++) {
		defaultShader->setFloat("slice", i);
		FBO->switchLayer(bufferTex, i);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	//Buoyancy
	buoyancyShader->use();
	buoyancyShader->setFloat("speed", .2f);
	buoyancyShader->setFloat("ambient", 0.0f);
	buoyancyShader->setFloat("cellSize", cell_size);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, bufferTex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_3D, temperatureTex);
	buoyancyShader->setInt("inVelocity", 0);
	buoyancyShader->setInt("inTemperature", 1);
	for (int i = 0; i < grid_size + 2; i++) {
		buoyancyShader->setFloat("slice", i);
		FBO->switchLayer(velocityInputTex, i);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	///* Color (based on temperature) */
	////Copy to buffer
	//glBindVertexArray(VAO);
	//glBindFramebuffer(GL_FRAMEBUFFER, writeFBO);
	//defaultShader->use();
	//glActiveTexture(GL_TEXTURE0);
	////glBindTexture(GL_TEXTURE_3D, temperatureTex); output of render flat?
	//glDrawArrays(GL_TRIANGLES, 0, 6);
	////Color
	//colorShader->use();
	//colorShader->setInt("inTemperature", 1);
	//for (int i = 0; i < grid_size + 2; i++) {
	//	colorShader->setFloat("slice", i);
	//	FBO->switchLayer(colorOutputTex, i);
	//	glDrawArrays(GL_TRIANGLES, 0, 6);
	//}

	///* Directional */
	////Copy to buffer
	//glBindVertexArray(VAO);
	//glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	//defaultShader->use();
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_3D, velocityInputTex);
	//glDrawArrays(GL_TRIANGLES, 0, 6);
	////Directional
	//glBindFramebuffer(GL_FRAMEBUFFER, velocityInputFBO);
	//directionalShader->use();
	//directionalShader->setFloat("time", time + ((rand() % 10)/.5f));
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_3D, bufferFBO);
	//glDrawArrays(GL_TRIANGLES, 0, 6);

	/* Unbind */
	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_3D, 0);
}

void Grid::projectGPU(int iterations) {
	/* Calculate Divergence */
	glBindVertexArray(VAO);
	glBindFramebuffer(GL_FRAMEBUFFER, writeFBO);

	divergeShader->use();
	divergeShader->setFloat("cellSize", cell_size);
	divergeShader->setFloat("timeStep", timeStep);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, velocityInputTex);
	for (int i = 0; i < grid_size + 2; i++) {
		divergeShader->setFloat("slice", i);
		FBO->switchLayer(divergenceOutputTex, i);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	/* Pressure Loop */
	pressureShader->use();
	pressureShader->setFloat("cellSize", cell_size);
	pressureShader->setFloat("timeStep", timeStep);

	for (int i = 0; i < iterations; i++) {
		/* Copy Pressure to Buffer */
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, pressureOutputTex);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_3D, divergenceOutputTex);
		pressureShader->setInt("inPressure", 0);
		pressureShader->setInt("divergences", 1);
		for (int i = 0; i < grid_size + 2; i++) {
			pressureShader->setFloat("slice", i);
			FBO->switchLayer(bufferTex, i);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		/* Write to Pressure */
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, bufferTex);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_3D, divergenceOutputTex);
		pressureShader->setInt("inPressure", 0);
		pressureShader->setInt("divergences", 1);
		for (int i = 0; i < grid_size + 2; i++) {
			pressureShader->setFloat("slice", i);
			FBO->switchLayer(pressureOutputTex, i);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
	}

	/* Gradient Subtraction */
	//Copy Velocity to Buffer
	glBindVertexArray(VAO);
	defaultShader->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, velocityInputTex);
	for (int i = 0; i < grid_size + 2; i++) {
		defaultShader->setFloat("slice", i);
		FBO->switchLayer(bufferTex, i);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	//Subtract
	gradientShader->use();
	gradientShader->setFloat("cellSize", cell_size);
	gradientShader->setFloat("timeStep", timeStep);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, bufferTex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_3D, pressureOutputTex);
	gradientShader->setInt("inVelocity", 0);
	gradientShader->setInt("inPressure", 1);
	for (int i = 0; i < grid_size + 2; i++) {
		gradientShader->setFloat("slice", i);
		FBO->switchLayer(velocityInputTex, i);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	drawBoundary(0);

	/* Unbind */
	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_3D, 0);
}

void Grid::moveDye(float time) {
	glBindVertexArray(VAO);
	glBindFramebuffer(GL_FRAMEBUFFER, writeFBO);

	/* Copy Old Fuel Distribution to Buffer */
	defaultShader->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, fuelOutputTex);
	for (int i = 0; i < grid_size + 2; i++) {
		defaultShader->setFloat("slice", i);
		FBO->switchLayer(bufferTex, i);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	/* Fill Source */
	fillShader->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, bufferTex);
	vec4 center = vec4(0.f, -0.75f, 0.f, 0.f);
	fillShader->setVec4("center", center);
	fillShader->setFloat("radius", 0.2f);
	fillShader->setFloat("cellSize", cell_size);
	for (int i = 0; i < grid_size + 2; i++) {
		fillShader->setFloat("slice", i);
		FBO->switchLayer(fuelOutputTex, i);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}


	/* Copy Old Fuel Distribution to Buffer */
	defaultShader->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, fuelOutputTex);
	for (int i = 0; i < grid_size + 2; i++) {
		defaultShader->setFloat("slice", i);
		FBO->switchLayer(bufferTex, i);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	/* Consume Fuel */
	fuelShader->use();
	fuelShader->setFloat("timeStep", timeStep);
	fuelShader->setFloat("rate", 0.15f);
	fuelShader->setFloat("cellSize", cell_size);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, bufferTex);
	for (int i = 0; i < grid_size + 2; i++) {
		fuelShader->setFloat("slice", i);
		FBO->switchLayer(fuelOutputTex, i);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	/* Copy Old Fuel Distribution to Buffer */
	defaultShader->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, fuelOutputTex);
	for (int i = 0; i < grid_size + 2; i++) {
		defaultShader->setFloat("slice", i);
		FBO->switchLayer(bufferTex, i);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	/* Advect Fuel */
	advectShader->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, velocityInputTex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_3D, bufferTex);
	advectShader->setInt("inVelocity", 0);
	advectShader->setInt("inQuantity", 1);
	advectShader->setFloat("cellSize", cell_size);
	advectShader->setFloat("timeStep", timeStep);
	for (int i = 0; i < grid_size + 2; i++) {
		advectShader->setFloat("slice", i);
		FBO->switchLayer(fuelOutputTex, i);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	/* Diffusion Loop */
	glBindVertexArray(VAO);
	diffuseShader->use();
	diffuseShader->setFloat("viscosity", viscosity);
	diffuseShader->setFloat("cellSize", cell_size);
	diffuseShader->setFloat("timeStep", timeStep);
	for (int i = 0; i < 0; i++) {
		/* Buffer -> Diffusion */
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, fuelOutputTex);
		for (int i = 0; i < grid_size + 2; i++) {
			diffuseShader->setFloat("slice", i);
			FBO->switchLayer(bufferTex, i);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		/* Diffusion -> Buffer */
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, bufferTex);
		for (int i = 0; i < grid_size + 2; i++) {
			diffuseShader->setFloat("slice", i);
			FBO->switchLayer(fuelOutputTex, i);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
	}

	/* Use new fuel values to update temperature */
	defaultShader->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, fuelOutputTex);
	for (int i = 0; i < grid_size + 2; i++) {
		defaultShader->setFloat("slice", i);
		FBO->switchLayer(temperatureTex, i);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	/* Unbind */
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindVertexArray(0);
}

void Grid::drawBoundary(int type) {
	/* Copy to Buffer */
	glBindVertexArray(VAO);
	glBindFramebuffer(GL_FRAMEBUFFER, writeFBO);

	defaultShader->use();
	//VELOCITY
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, velocityInputTex);
	for (int i = 0; i < grid_size + 2; i++) {
		defaultShader->setFloat("slice", i);
		FBO->switchLayer(bufferTex, i);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	//PRESSURE
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, pressureOutputTex);
	for (int i = 0; i < grid_size + 2; i++) {
		defaultShader->setFloat("slice", i);
		FBO->switchLayer(buffer2Tex, i);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	/* Draw Boundary */
	vec4 top, bottom, left, right, up, down;
	top = vec4(0.f, -cell_size / 2.f, 0.f, 0.f);
	bottom = vec4(0.f, cell_size / 2.f, 0.f, 0.f);
	left = vec4(cell_size / 2.f, 0.f, 0.f, 0.f);
	right = vec4(-cell_size / 2.f, 0.f, 0.f, 0.f);
	up = vec4(0.f, 0.f, cell_size / 2.f, 0.f);
	down = vec4(0.f, 0.f, -cell_size / 2.f, 0.f);

	glBindVertexArray(bVAO);
	borderShader->use();
	borderShader->setFloat("cellSize", cell_size);

	//VELOCITY
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, bufferTex);
	borderShader->setFloat("scalar", -1.f);
	for (int i = 0; i < grid_size + 2; i++) {
		borderShader->setFloat("slice", i);
		FBO->switchLayer(velocityInputTex, i);

		borderShader->setVec4("offset", left);
		glDrawArrays(GL_LINES, 0, 2);
		borderShader->setVec4("offset", top);
		glDrawArrays(GL_LINES, 1, 2);
		borderShader->setVec4("offset", right);
		glDrawArrays(GL_LINES, 2, 2);
		borderShader->setVec4("offset", bottom);
		glDrawArrays(GL_LINES, 3, 3);
	}

	//PRESSURE
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, buffer2Tex);
	borderShader->setFloat("scalar", 1.f);
	for (int i = 0; i < grid_size + 2; i++) {
		borderShader->setFloat("slice", i);
		FBO->switchLayer(pressureOutputTex, i);

		borderShader->setVec4("offset", left);
		glDrawArrays(GL_LINES, 0, 2);
		borderShader->setVec4("offset", top);
		glDrawArrays(GL_LINES, 1, 2);
		borderShader->setVec4("offset", right);
		glDrawArrays(GL_LINES, 2, 2);
		borderShader->setVec4("offset", bottom);
		glDrawArrays(GL_LINES, 3, 3);
	}

	//Top and Bottom Slices
	glBindVertexArray(VAO);
	borderShader->setFloat("slice", 0.f);
	borderShader->setVec4("offset", up);
	borderShader->setFloat("scalar", -1.f);
	FBO->switchLayer(velocityInputTex, 0);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	FBO->switchLayer(pressureOutputTex, 0);
	borderShader->setFloat("scalar", 1.f);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	borderShader->setFloat("slice", grid_size + 1);
	borderShader->setVec4("offset", down);
	borderShader->setFloat("scalar", -1.f);
	FBO->switchLayer(velocityInputTex, grid_size + 1);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	FBO->switchLayer(pressureOutputTex, grid_size + 1);
	borderShader->setFloat("scalar", 1.f);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	/* Unbind */
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindVertexArray(0);
}

/* ####################################### */

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