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
	bool GPUsim;

	/* Setup */
	void init();

	void toggleFlameColor();

	void toggleTexture();

	void toggleScene();

	/* Run Simulation */
	void simulate(float time, vec2 mousepos, bool click);

	void drawContents();
	void render();

	/* Interaction */
	void moveCamera(vec3 moveBy);
	void rotateCamera(double deltaX, double deltaY);
	void resetCamera();
	void changeScrDimensions(int width, int height);

	void bindScreenVertices();

private:
	
	//Output Texture
	GLuint screenTex, screenFBO;

	//Object 2 World
	float rotateX, rotateY;

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
	Shader *renderShader, *renderShaderRay, *cellShader;
	Grid *grid;
	
	float SCR_HEIGHT;
	float SCR_WIDTH;
	float resolution;
	int fireColor;
};

#endif