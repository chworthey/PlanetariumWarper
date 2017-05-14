#pragma once

#include <GL/glew.h>
#include <map>

class Shader
{
private:
    GLuint m_program;
    GLuint m_vertexShader;
    GLuint m_fragmentShader;

public:
    Shader();
    ~Shader();
    bool LoadShader(const std::string &filename, GLenum shaderType);

    GLuint GetVertexShaderHandle();
    GLuint GetFragmentShaderHandle();
    GLuint GetProgramHandle();

    void Enable();
    void Disable();

private:
    Shader(const Shader &) = delete;
    Shader &operator=(const Shader &) = delete;
};