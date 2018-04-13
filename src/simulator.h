#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "camera.h"
#include "shader.h"
#include "grid.h"

class Simulator {
	
public:

	Simulator(float height, float width) {
		SCR_HEIGHT = height;
		SCR_WIDTH = width;
	}
	void init();

	void bindVertices(float *vertices, int size);
	void drawContents();
	void moveCamera(vec3 moveBy);

private:
	GLuint VBO;
	GLuint VAO;

	Camera *CAMERA;
	Shader *shader;
	Grid *grid;
	
	unsigned int SCR_HEIGHT;
	unsigned int SCR_WIDTH;
};

#endif