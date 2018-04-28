#include "framebuffer.h"

#include <glad/glad.h>
#include <iostream>

GLuint Framebuffer::createFBO (int texID) {
	FBO.push_back(0);
	glGenFramebuffers(1, &FBO[FBO.size() - 1]);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO[FBO.size() - 1]);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texID, 0);
	getErrors();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return FBO[FBO.size() - 1];
}

GLuint Framebuffer::create3DFBO(int texID) {
	FBO.push_back(0);
	glGenFramebuffers(1, &FBO[FBO.size() - 1]);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO[FBO.size() - 1]);
	glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texID, 0, 0);
	getErrors();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return FBO[FBO.size() - 1];
}

void Framebuffer::switchLayer(int texID, int layer) {
	glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texID, 0, layer);
}

GLuint Framebuffer::createEmptyTexture(int size) {
	//DEPRECATED, DO NOT USE
	TEX.push_back(0);
	glGenTextures(1, &TEX[TEX.size() - 1]);
	glBindTexture(GL_TEXTURE_2D, TEX[TEX.size() - 1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	getErrors();
	return TEX[TEX.size() - 1];
}

GLuint Framebuffer::createTexture(int size, float *data) {
	TEX.push_back(0);
	glGenTextures(1, &TEX[TEX.size() - 1]);
	glBindTexture(GL_TEXTURE_2D, TEX[TEX.size() - 1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, size, size, 0, GL_RGB, GL_FLOAT, data);
	getErrors();
	return TEX[TEX.size() - 1];
}

GLuint Framebuffer::create3DTexture(int size, float *data) {
	TEX.push_back(0);
	glGenTextures(1, &TEX[TEX.size() - 1]);
	glBindTexture(GL_TEXTURE_3D, TEX[TEX.size() - 1]);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB32F, size, size, size, 0, GL_RGB, GL_FLOAT, data);
	getErrors();
	return TEX[TEX.size() - 1];
}

GLuint Framebuffer::create3DTexture(int size) {
	TEX.push_back(0);
	glGenTextures(1, &TEX[TEX.size() - 1]);
	glBindTexture(GL_TEXTURE_3D, TEX[TEX.size() - 1]);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB32F, size, size, size, 0, GL_RGB, GL_FLOAT, NULL);
	getErrors();
	return TEX[TEX.size() - 1];
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