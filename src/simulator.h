#ifndef GRID_H
#define GRID_H

#include "camera.h"
#include "shader.h"

class Simulator {
	
public:

	Simulator(float height, float width) {
		SCR_HEIGHT = height;
		SCR_WIDTH = width;
	}
	void init();

	void bindVertices();
	void drawContents();
	void moveCamera(vec3 moveBy);

private:
	GLuint VBO;
	GLuint VAO;

	Camera *CAMERA;
	Shader *shader;
	
	unsigned int SCR_HEIGHT;
	unsigned int SCR_WIDTH;

};

#endif