#pragma once

#include <GL\glew.h>

class Texture
{
private:
    GLuint m_texture;

public:
    void Load(std::string filename);
    GLuint GetTextureHandle();
};