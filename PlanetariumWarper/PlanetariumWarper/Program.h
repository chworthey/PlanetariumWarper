#pragma once

struct ProgramConfiguration;
class InputState;

class Program
{
private:
    bool m_exit;

public:
    Program();

    void Initialize(ProgramConfiguration &config);
    void Load();
    void Update(const InputState &state);
    void Draw();
    bool ReadyToExit() const;

protected:
    void Exit();
};