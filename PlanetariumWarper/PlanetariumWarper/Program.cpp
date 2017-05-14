#include "stdafx.h"
#include "Program.h"

#include "InputState.h"
#include "ProgramConfiguration.h"
#include <GL\glew.h>

Program::Program()
    : m_exit(false)
{
}

void Program::Initialize(ProgramConfiguration &config)
{
    config.SetTitle("Planetarium Warper");
    config.SetWindowWidth(512);
    config.SetWindowHeight(512);
    config.SetGLMajorVersion(3);
    config.SetGLMinorVersion(2);
}

void Program::Load()
{
    glewInit();
}

void Program::Update(const InputState &inputState)
{
    if (inputState.IsKeyPressed(InputKey::ESCAPE))
        Exit();
}

void Program::Draw()
{
}

bool Program::ReadyToExit() const
{
    return m_exit;
}

void Program::Exit()
{
    m_exit = true;
}
