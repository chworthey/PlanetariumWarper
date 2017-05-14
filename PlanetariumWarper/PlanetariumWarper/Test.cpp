#include "stdafx.h"
/*#include "Test.h"

#include "InputState.h"
#include "ProgramConfiguration.h"
#include <GL\glew.h>
#include <iostream>

#define WINDOW_WIDTH 512
#define WINDOW_HEIGHT 512

void
ETB_GL_ERROR_CALLBACK(GLenum        source,
    GLenum        type,
    GLuint        id,
    GLenum        severity,
    GLsizei       length,
    const GLchar* message,
    GLvoid*       userParam)
{
    std::cout << reinterpret_cast<const char*>(message) << std::endl;
}

Program::Program()
    : m_exit(false)
{
}

void Program::Initialize(ProgramConfiguration &config)
{
    config.SetTitle("Planetarium Warper");
    config.SetWindowWidth(WINDOW_WIDTH);
    config.SetWindowHeight(WINDOW_HEIGHT);
    config.SetGLMajorVersion(2);
    config.SetGLMinorVersion(0);
}

GLuint LoadShader(GLenum type, const GLchar *shaderSrc)
{
    GLuint shader;
    GLint compiled;

    shader = glCreateShader(type);

    if (shader == 0)
    {
        return 0;
    }

    // Load the shader source

    glShaderSource(shader, 1, &shaderSrc, NULL);

    // Compile the shader
    glCompileShader(shader);

    // Check the compile status
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

    if (!compiled)
    {
        GLint infoLen = 0;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

        if (infoLen > 1)
        {
            char* infoLog = (char*)malloc(sizeof(char) * infoLen);

            glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
            free(infoLog);
        }

        glDeleteShader(shader);
        return 0;
    }

    return shader;

}

void Program::Load()
{
    glewExperimental = true;
    glewInit();

    const char vertexShaderString[] =
        "attribute vec4 vPosition;    \n"
        "void main()                  \n"
        "{                            \n"
        "   gl_Position = vPosition;  \n"
        "}                            \n";

    const char fragmentShaderString[] =
        "precision mediump float;\n"\
        "void main()                                  \n"
        "{                                            \n"
        "  gl_FragColor = vec4 ( 1.0, 0.0, 0.0, 1.0 );\n"
        "}                                            \n";

    GLuint vertexShader;
    GLuint fragmentShader;
    GLint linked;

    vertexShader = LoadShader(GL_VERTEX_SHADER, vertexShaderString);
    fragmentShader = LoadShader(GL_FRAGMENT_SHADER, fragmentShaderString);

    programObject = glCreateProgram();

    if (programObject == 0) {

        return;

    }

    glAttachShader(programObject, vertexShader);
    glAttachShader(programObject, fragmentShader);
    glBindAttribLocation(programObject, 0, "vPosition");
    glLinkProgram(programObject);

    glGetProgramiv(programObject, GL_LINK_STATUS, &linked);

    if (!linked) {

        GLint infoLen = 0;

        glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLen);

        if (infoLen > 1) {

            char* infoLog = (char*)malloc(sizeof(char) * infoLen);
            glGetProgramInfoLog(programObject, infoLen, NULL, infoLog);
            free(infoLog);

        }

        glDeleteProgram(programObject);
        return;

    }

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    return;
}

void Program::Update(const InputState &inputState)
{
    if (inputState.IsKeyPressed(InputKey::ESCAPE))
    {
        Exit();
    }
}

void Program::Draw()
{
    glClearColor(255.0f, 0.0f, 255.0f, 1);
    GLfloat vVertices[] = {
        0.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f
    };

    glViewport(0, 0, 320, 480);

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(programObject);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vVertices);

    glEnableVertexAttribArray(0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

bool Program::ReadyToExit() const
{
    return m_exit;
}

void Program::Exit()
{
    m_exit = true;
    Unload();
}

void Program::Unload()
{

    glUseProgram(0);
}
*/