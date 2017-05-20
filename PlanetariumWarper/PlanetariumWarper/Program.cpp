
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
    config.SetGLMajorVersion(3);
    config.SetGLMinorVersion(3);
}


void Program::Load()
{
    glewExperimental = GL_TRUE;
    glewInit();

    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
    glDebugMessageCallback((GLDEBUGPROC)ETB_GL_ERROR_CALLBACK, nullptr);

    m_shader.LoadShader("FullscreenQuad.vert", GL_VERTEX_SHADER);
    m_shader.LoadShader("FullscreenQuad.frag", GL_FRAGMENT_SHADER);

    GLuint prog = m_shader.GetProgramHandle();
    m_shader.Link();
    m_shader.Enable();

    float quad[] =
    {
        // Positions
        -1.0f, 1.0f, // v0 - top left corner
        -1.0f, -1.0f, // v1 - bottom left corner
        1.0f, 1.0f,  // v2 - top right corner
        1.0f, -1.0f,   // v3 - bottom right corner

        // UV's
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 1.0f,
        1.0f, 0.0f
    };

    glGenVertexArrays(1, &m_vertexArrayObject);
    glBindVertexArray(m_vertexArrayObject);


    glBindAttribLocation(prog, 0, "vPosition");
    glBindAttribLocation(prog, 1, "vUV");

    // Create the Vertex Buffer Object for the full screen quad.

    glGenBuffers(1, &m_vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(8 * sizeof(GL_FLOAT)));

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
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
    glClear(GL_COLOR_BUFFER_BIT);

    m_shader.Enable();
    glBindVertexArray(m_vertexArrayObject);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
    m_shader.Disable();
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
