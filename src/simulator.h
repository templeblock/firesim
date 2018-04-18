#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <vector>

#include "camera.h"
#include "shader.h"
#include "grid.h"
#include "framebuffer.h"

class Simulator {
	
public:

	Simulator(float width, float height) {
		SCR_HEIGHT = height;
		SCR_WIDTH = width;
	}

	enum RenderType {
		VELOCITY,
		PRESSURE,
		DIVERGENCE,
		INK
	};
	RenderType current;

	/* Setup */
	void init();
	void bindVertices();
	void bindVertexSet(unsigned int index, unsigned int pointer, const std::vector<float> vertices);

	/* Run Simulation */
	void simulate(float time);

	void drawContents();
	void drawTexture();
	void drawGrid();
	void drawArrows();

	/* Interaction */
	void moveCamera(vec3 moveBy);
	void changeScrDimensions(int width, int height);

	void generateTextures();

	void updateVelocityTexture(bool normalized);

	void updatePressureTexture();

	void updateDivergenceTexture();

private:
	
	bool normalized_renders;
	Framebuffer *FBO;

	std::vector<GLuint> gridVBO;
	std::vector<GLuint> gridVAO;

	GLuint velocityVBO;
	GLuint velocityVAO;

	GLuint screenVBO;
	GLuint screenVAO;

	GLuint currentTex;

	Camera *CAMERA;
	Shader *gridShader;
	Shader *cellShader;
	Grid *grid;
	
	float SCR_HEIGHT;
	float SCR_WIDTH;
};

#endif