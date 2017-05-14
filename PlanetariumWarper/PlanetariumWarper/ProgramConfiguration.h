#pragma once

struct ProgramConfiguration
{
private:
    std::string m_title;
    int m_windowWidth;
    int m_windowHeight;
    int m_glMajorVersion;
    int m_glMinorVersion;

public:
    ProgramConfiguration()
        : m_title("Untitled"),
        m_windowWidth(512),
        m_windowHeight(512),
        m_glMajorVersion(3),
        m_glMinorVersion(2)
    { }

    void SetTitle(const std::string &title)
    {
        m_title = title;
    }

    const std::string &GetTitle() const
    {
        return m_title;
    }

    void SetWindowWidth(int width)
    {
        m_windowWidth = width;
    }

    int GetWindowWidth() const
    {
        return m_windowWidth;
    }

    void SetWindowHeight(int height)
    {
        m_windowHeight = height;
    }

    int GetWindowHeight() const
    {
        return m_windowHeight;
    }

    void SetGLMajorVersion(int version)
    {
        m_glMajorVersion = version;
    }

    int GetGLMajorVersion() const
    {
        return m_glMajorVersion;
    }

    void SetGLMinorVersion(int version)
    {
        m_glMinorVersion = version;
    }

    int GetGLMinorVersion() const
    {
        return m_glMinorVersion;
    }
};