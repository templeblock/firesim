#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <glad/glad.h>
#include <vector>

class Framebuffer {
public:

	Framebuffer () {
		VEL_TEX = std::vector<GLuint>(0);
	}

	void createFBO();

	void createEmptyTexture(int size);
	void createTexture(int size, float *data);

	void changeTextureImage(int size, GLuint ID, float * data);

	void getErrors();

	std::vector<GLuint> VEL_FBO;
	std::vector<GLuint> VEL_TEX;
};

#endif FRAMEBUFFER_H
