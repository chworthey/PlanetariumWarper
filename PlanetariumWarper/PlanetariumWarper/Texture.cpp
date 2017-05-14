#include "stdafx.h"
#include "Texture.h"
#include "soil.h"

void Texture::Load(std::string filename)
{
    m_texture = SOIL_load_OGL_texture(filename.c_str(), 3, 0, SOIL_FLAG_POWER_OF_TWO);
}

GLuint Texture::GetTextureHandle()
{
    return m_texture;
}
