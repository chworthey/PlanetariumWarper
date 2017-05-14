#include "stdafx.h"
#include "InputState.h"

void InputState::AddMouseButtonPressed(InputMouseButton button)
{
    m_pressedMouseButtons.emplace(button);
}

bool InputState::IsMouseButtonPressed(InputMouseButton button) const
{
    return m_pressedMouseButtons.find(button) != m_pressedMouseButtons.end();
}

void InputState::AddKeyPressed(InputKey key)
{
    m_pressedKeys.emplace(key);
}

bool InputState::IsKeyPressed(InputKey key) const
{
    return m_pressedKeys.find(key) != m_pressedKeys.end();
}

void InputState::SetMousePosition(const glm::vec2 & position)
{
    m_mousePos = position;
}

const glm::vec2 & InputState::GetMousePosition() const
{
    return m_mousePos;
}

void InputState::SetAmountMouseWheelScrolled(const glm::vec2 & scrollAmount)
{
    m_amountMouseWheelScrolled = scrollAmount;
}

const glm::vec2 & InputState::GetAmountMouseWheelScrolled() const
{
    return m_amountMouseWheelScrolled;
}

InputKey InputState::GetKeyFromScancode(short scancode)
{
    return (InputKey)scancode;
}
