#include "stdafx.h"
#include <SDL.h>

#include <iostream>
#include <map>
#include "Program.h"
#include "ProgramConfiguration.h"
#include "InputState.h"

#define ERROR_CODE_SUCCESS 0
#define ERROR_CODE_FAIL 1

void processSDLEvents(bool &shouldQuit, InputState &inputState);
void processSDLMouseButtonEvent(InputState &inputState, const SDL_Event &ev);

int main(int argc, char *args[])
{
    Program prog;
    ProgramConfiguration config;
    prog.Initialize(config);

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "Failed to init SDL, Error: " << SDL_GetError() << std::endl;
        return ERROR_CODE_FAIL;
    }

    SDL_Window *win = SDL_CreateWindow(
        config.GetTitle().c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        config.GetWindowWidth(),
        config.GetWindowHeight(),
        SDL_WINDOW_OPENGL
    );

    if (!win)
    {
        std::cerr << "Failed to create window, Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return ERROR_CODE_FAIL;
    }

    SDL_Surface *surface = SDL_GetWindowSurface(win);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE); // No deprecated gl calls!
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, config.GetGLMajorVersion());
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, config.GetGLMinorVersion());
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, true);

    SDL_GLContext context = SDL_GL_CreateContext(win);

    if (!context)
    {
        std::cerr << "Could not create GL context, Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return ERROR_CODE_FAIL;
    }

    prog.Load();

    bool quit = false;
    while (!prog.ReadyToExit() && !quit)
    {
        InputState inputState;
        processSDLEvents(quit, inputState);

        prog.Update(inputState);
        prog.Draw();
        SDL_GL_SwapWindow(win);
    }

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(win);
    SDL_Quit();

    return ERROR_CODE_SUCCESS;
}

void processSDLEvents(bool &shouldQuit, InputState &inputState)
{
    SDL_Event ev;
    while (SDL_PollEvent(&ev))
    {
        switch (ev.type)
        {
        case SDL_QUIT:
            shouldQuit = true;
            break;
        case SDL_KEYDOWN:
            inputState.AddKeyPressed(InputState::GetKeyFromScancode(static_cast<short>(ev.key.keysym.scancode)));
            break;
        case SDL_MOUSEBUTTONDOWN:
            processSDLMouseButtonEvent(inputState, ev);
            break;
        case SDL_MOUSEWHEEL:
            inputState.SetAmountMouseWheelScrolled(std::move(glm::vec2(ev.wheel.x, ev.wheel.y)));
            break;
        default:
            break;
        }
    }

    int x, y;
    SDL_GetMouseState(&x, &y);
    inputState.SetMousePosition(std::move(glm::vec2(x, y)));
}

void processSDLMouseButtonEvent(InputState &inputState, const SDL_Event &ev)
{
    switch (ev.button.button)
    {
    case SDL_BUTTON_LEFT:
        inputState.AddMouseButtonPressed(InputMouseButton::Left);
        break;
    case SDL_BUTTON_MIDDLE:
        inputState.AddMouseButtonPressed(InputMouseButton::Middle);
        break;
    case SDL_BUTTON_RIGHT:
        inputState.AddMouseButtonPressed(InputMouseButton::Right);
        break;
    case SDL_BUTTON_X1:
        inputState.AddMouseButtonPressed(InputMouseButton::X1);
        break;
    case SDL_BUTTON_X2:
        inputState.AddMouseButtonPressed(InputMouseButton::X2);
        break;
    default:
        break;
    }
}