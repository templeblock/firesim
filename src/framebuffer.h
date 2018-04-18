#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <glad/glad.h>
#include <vector>

class Framebuffer {
public:

	Framebuffer () {
		FBO = std::vector<GLuint>(0);
		TEX = std::vector<GLuint>(0);
	}

	GLuint createFBO(int texID);

	GLuint createEmptyTexture(int size);
	GLuint createTexture(int size, float *data);

	void changeTextureImage(int size, GLuint ID, float * data);

	void getErrors();

	std::vector<GLuint> FBO;
	std::vector<GLuint> TEX;
};

#endif FRAMEBUFFER_H
