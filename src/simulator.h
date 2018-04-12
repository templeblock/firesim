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

	void drawContents();
	void moveCamera(vec3 moveBy);

	Camera *CAMERA;
	Shader *shader;

	unsigned int SCR_HEIGHT;
	unsigned int SCR_WIDTH;

private:

	//who the fuck

};

#endif