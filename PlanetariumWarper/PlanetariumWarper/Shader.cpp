#include "stdafx.h"
#include "Shader.h"
#include <fstream>

Shader::Shader()
    : m_program(0),
    m_fragmentShader(0),
    m_vertexShader(0)
{
}

namespace
{
    void deleteShader(GLuint shader)
    {
        if (!shader)
        {
            return;
        }

        GLint flaggedForDelete = GL_FALSE;

        glGetShaderiv(shader, GL_DELETE_STATUS, &flaggedForDelete);

        if (!flaggedForDelete)
        {
            glDeleteShader(shader);
        }
    }
}

Shader::~Shader()
{
    if (m_program)
    {
        deleteShader(m_vertexShader);
        deleteShader(m_fragmentShader);

        glDeleteProgram(m_program);
        m_program = 0;
    }
}

namespace
{
    GLuint createProgram()
    {
        GLuint program = glCreateProgram();

        if (glIsProgram(program))
            return program;
        else
            return 0;
    }
}

bool Shader::LoadShader(const std::string & filename, GLenum shaderType)
{
    bool loaded = false;
    std::ifstream file(filename, std::ios::binary);

    if (file.is_open())
    {
        file.seekg(0, std::ios::end);

        unsigned int fileSize = static_cast<unsigned int>(file.tellg());
        std::string source;

        source.resize(fileSize);
        file.seekg(0, std::ios::beg);
        file.read(&source[0], fileSize);

        GLuint shader = glCreateShader(shaderType);

        if (glIsShader(shader) == GL_TRUE)
        {
            const GLchar *pszSource = reinterpret_cast<const GLchar *>(source.c_str());
            GLint length = 0;

            glShaderSource(shader, 1, &pszSource, 0);
            glGetShaderiv(shader, GL_SHADER_SOURCE_LENGTH, &length);

            if (length > 0)
            {
                GLint isCompiled = GL_FALSE;

                glCompileShader(shader);
                glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);

                if (isCompiled == GL_FALSE)
                {
                    glDeleteShader(shader);
                    shader = 0;
                }
                else
                {
                    if (!m_program)
                    {
                        m_program = createProgram();
                    }
                    glAttachShader(m_program, shader);

                    switch (shaderType)
                    {
                    case GL_FRAGMENT_SHADER:
                        m_fragmentShader = shader;
                        break;

                    case GL_VERTEX_SHADER:
                        m_vertexShader = shader;
                        break;

                    default:
                        break;
                    }

                    loaded = true;
                }
            }
        }
    }

    return loaded;
}

GLuint Shader::GetVertexShaderHandle()
{
    return m_vertexShader;
}

GLuint Shader::GetFragmentShaderHandle()
{
    return m_fragmentShader;
}

GLuint Shader::GetProgramHandle()
{
    return m_program;
}

void Shader::Enable()
{
    glUseProgram(m_program);
}

void Shader::Disable()
{
    glUseProgram(0);
}
