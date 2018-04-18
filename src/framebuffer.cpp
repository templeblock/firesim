#include "framebuffer.h"

#include <glad/glad.h>
#include <iostream>

void Framebuffer::createFBO () {
	/*glGenFramebuffers(1, &VEL_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, VEL_FBO);
	getErrors();*/
}

void Framebuffer::createEmptyTexture(int size) {
	VEL_TEX.push_back(0);
	glGenTextures(1, &VEL_TEX[VEL_TEX.size() - 1]);
	glBindTexture(GL_TEXTURE_2D, VEL_TEX[VEL_TEX.size() - 1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	getErrors();
}

void Framebuffer::createTexture(int size, float *data) {
	VEL_TEX.push_back(0);
	glGenTextures(1, &VEL_TEX[VEL_TEX.size() - 1]);
	glBindTexture(GL_TEXTURE_2D, VEL_TEX[VEL_TEX.size() - 1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, size, size, 0, GL_RGB, GL_FLOAT, data);
	getErrors();
}

void Framebuffer::changeTextureImage(int size, GLuint ID, float *data) {
	glBindTexture(GL_TEXTURE_2D, ID);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size, size, GL_RGB, GL_FLOAT, data);
	getErrors();
}

void Framebuffer::getErrors() {
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		// Process/log the error.
		std::cout << "GL Error:" << err << std::endl;
	}
}