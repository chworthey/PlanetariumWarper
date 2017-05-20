#pragma once

#include <GL/glew.h>
#include "Shader.h"

struct ProgramConfiguration;
class InputState;

class Program
{
private:
    bool m_exit;
    Shader m_shader;
    GLuint m_vertexArrayObject;
    GLuint m_vertexBufferObject;

public:
    Program();

    void Initialize(ProgramConfiguration &config);
    void Load();
    void Update(const InputState &state);
    void Draw();
    bool ReadyToExit() const;

protected:
    void Exit();
    void Unload();
};
