
#include "stdafx.h"
#include "Program.h"

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


void Program::Load()
{
    glewExperimental = GL_TRUE;
    glewInit();

    std::cout << glGetError() << std::endl;

    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
    glDebugMessageCallback((GLDEBUGPROC)ETB_GL_ERROR_CALLBACK, nullptr);

    m_shader.LoadShader("FullscreenQuad.vert", GL_VERTEX_SHADER);
    m_shader.LoadShader("FullscreenQuad.frag", GL_FRAGMENT_SHADER);

    std::cout << glGetError() << std::endl;

    float quad[] =
    {
        -1.0f,  1.0f,	// v0 - top left corner
        -1.0f, -1.0f,	// v1 - bottom left corner
        1.0f,  1.0f,	// v2 - top right corner
        1.0f, -1.0f	// v3 - bottom right corner
    };

    glGenVertexArrays(1, &m_vertexArrayObject);
    glBindVertexArray(m_vertexArrayObject);

    std::cout << glGetError() << std::endl;

    glGenBuffers(1, &m_vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

    std::cout << glGetError() << std::endl;

    auto prog = m_shader.GetProgramHandle();
    auto location = glGetAttribLocation(prog, "PositionNDC");
    glVertexAttribPointer(location, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(location);

    std::cout << glGetError() << std::endl;

    glBindVertexArray(0);
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
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    std::cout << glGetError() << std::endl;
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    std::cout << glGetError() << std::endl;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    std::cout << glGetError() << std::endl;

    m_shader.Enable();
    std::cout << glGetError() << std::endl;
    glBindVertexArray(m_vertexArrayObject);
    std::cout << glGetError() << std::endl;
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);
    std::cout << glGetError() << std::endl;
    glBindVertexArray(0);
    std::cout << glGetError() << std::endl;
    m_shader.Disable();

    std::cout << glGetError() << std::endl;
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
    if (m_vertexArrayObject)
    {
        glDeleteVertexArrays(1, &m_vertexArrayObject);
        m_vertexArrayObject = 0;
    }

    if (m_vertexBufferObject)
    {
        glDeleteBuffers(1, &m_vertexBufferObject);
        m_vertexBufferObject = 0;
    }

    glUseProgram(0);
}
