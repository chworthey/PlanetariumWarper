
#include "stdafx.h"
#include "Program.h"

#include "InputState.h"
#include "ProgramConfiguration.h"
#include <GL\glew.h>
#include <iostream>

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 1024

#define USE_WARP_INPUT_TEX 0

// yo, adrian

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
    config.SetGLMinorVersion(0); // Latest SOIL-supported version
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
    m_texture.LoadImage("radialgrid.tga");

#if USE_WARP_INPUT_TEX
    m_warpTexture.LoadUVSet("xyuv4k.data");
#endif

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

    glBindTexture(GL_TEXTURE_2D, m_texture.GetTextureHandle());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    GLint samplerLoc = glGetUniformLocation(prog, "texSampler");
    glUniform1i(samplerLoc, 0);

#if USE_WARP_INPUT_TEX
    glBindTexture(GL_TEXTURE_2D, m_warpTexture.GetTextureHandle());
    GLint warpSamplerLoc = glGetUniformLocation(prog, "warpSampler");
    glUniform1i(warpSamplerLoc, 1);
#endif

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

    glEnable(GL_TEXTURE_2D);

    m_shader.Enable();
    glBindVertexArray(m_vertexArrayObject);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    GLuint tex = m_texture.GetTextureHandle();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    GLint samplerLoc = glGetUniformLocation(m_shader.GetProgramHandle(), 
        "texSampler");
    glUniform1i(samplerLoc, 0);

#if USE_WARP_INPUT_TEX
    GLuint warpTex = m_warpTexture.GetTextureHandle();
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, warpTex);
    GLint warpSamplerLoc = glGetUniformLocation(m_shader.GetProgramHandle(),
        "warpSampler");
    glUniform1i(warpSamplerLoc, 1);
#endif

    //@dad: attention here, we will use shader uniforms to set parameters in
    // the vert/frag shaders (see FullscreenQuad.vert/frag) like this
    GLint scaleLoc = glGetUniformLocation(m_shader.GetProgramHandle(),
        "uvScale");
    glUniform2f(scaleLoc, 1.0f, -1.0f);
    GLint offsetLoc = glGetUniformLocation(m_shader.GetProgramHandle(),
        "uvOffset");
    glUniform2f(offsetLoc, 0.0f, 1.0f);

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
