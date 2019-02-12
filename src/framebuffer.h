#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <glad/glad.h>
#include <vector>

class Framebuffer {
public:
    Framebuffer()
    {
        FBO = std::vector<GLuint>(0);
        TEX = std::vector<GLuint>(0);
    }

    GLuint createFBO(int texID);

    GLuint create3DFBO(int texID);

    void switchLayer(int texID, int layer);

    GLuint createEmptyTexture(int size);

    GLuint createTexture(int size, float* data);

    GLuint create3DTexture(int size, float* data);

    GLuint create3DTexture(int size);

    void changeTextureImage(int size, GLuint ID, float* data);

    void getErrors();

    std::vector<GLuint> FBO;
    std::vector<GLuint> TEX;
};

#endif
