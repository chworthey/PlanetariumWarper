#pragma once

#include <GL\glew.h>

class Texture
{
private:
    GLuint m_texture;

public:
    void LoadImage(const std::string &filename);
    void LoadUVSet(const std::string &filename);
    GLuint GetTextureHandle();
};