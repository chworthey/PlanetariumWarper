#include "stdafx.h"
#include "Texture.h"
#include "soil.h"
#include <GL/glew.h>
#include <iostream>
#include <fstream>

void Texture::LoadImage(const std::string &filename)
{
    m_texture = SOIL_load_OGL_texture(filename.c_str(), 0, 0, SOIL_FLAG_POWER_OF_TWO);
}

void Texture::LoadUVSet(const std::string &filename)
{
    std::ifstream f = std::ifstream(filename);
    int tmp, width, height;
    f >> tmp >> width >> height;
    int totalSize = width * height;
    std::vector<float> dat = std::vector<float>(totalSize * 3);
    for (int n = 0, a = 0; n < totalSize; n++)
    {
        float throwAwayX, throwAwayY, U, V, intensity;
        f >> throwAwayX >> throwAwayY >> U >> V >> intensity;
        dat[a++] = intensity;
        dat[a++] = V;
        dat[a++] = U;
    }

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_FLOAT, &dat[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    m_texture = tex;
}

GLuint Texture::GetTextureHandle()
{
    return m_texture;
}
