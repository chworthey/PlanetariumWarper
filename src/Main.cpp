#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <engine/filesystem.h>
#include <fstream>
#include <iostream>
#include <vector>
#include "ConfigFile.h"
#include "GLText.h"
#include "GLWarper.h"
#include <stb_image_write.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

static std::shared_ptr<ConfigFile> config;
static bool hideText = false;
static bool fullscreen = false;
static GLFWwindow* window = nullptr;
static bool remakeWindow = false;
static GLText text;
static GLWarper warper;
static std::string configFilename = "config.json";

#define CLAMP(value, minimum, maximum) std::fmax(std::fmin(maximum, value), minimum)

void exportData() {

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    GLuint frameBuffer = 0;
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

    GLuint renderedTexture;
    glGenTextures(1, &renderedTexture);

    glBindTexture(GL_TEXTURE_2D, renderedTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, config->getExportSize(), config->getExportSize(), 0, GL_RGB, GL_FLOAT, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);

    GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, drawBuffers);

    glViewport(0, 0, config->getExportSize(), config->getExportSize());

    warper.SetRenderMode(GLWarperRenderMode::Export);
    warper.Render();
    warper.SetRenderMode(GLWarperRenderMode::Normal);

    std::vector<float> bytes = std::vector<float>(config->getExportSize() * config->getExportSize() * 3);

    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, config->getExportSize(), config->getExportSize(), 0);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, &bytes[0]);


    std::vector<char> pixels = std::vector<char>(config->getExportSize() * config->getExportSize() * 3);
    int n = 0;
    for (auto it : bytes) {
        pixels[n++] = static_cast<char>(it * 255.0f);
    }

    stbi_write_bmp("output.bmp", config->getExportSize(), config->getExportSize(), 3, &pixels[0]);

    glDeleteTextures(1, &renderedTexture);
    glDeleteFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, width, height);

    remakeWindow = true;

    std::ofstream file("output.xyuv");

    file << 2 << std::endl;
    file << config->getExportSize() << " " << config->getExportSize() << std::endl;
    
    for (int n = 0; n < config->getExportSize() * config->getExportSize(); ++n) {
        file << (bytes[n * 3] * 2.0f - 1.0f) * config->getAspectRatio() << " ";
        file << (bytes[n * 3 + 1] * 2.0f - 1.0f) << " ";
        file << ((n % config->getExportSize()) / static_cast<float>(config->getExportSize())) << " " ;
        file << ((n / config->getExportSize()) / static_cast<float>(config->getExportSize())) << " ";
        file << bytes[n * 3 + 2] << std::endl;
    }

    file.close();
}

void nudge(bool positive) {
    float scalar = positive ? 1.0f : -1.0f;

    bool shiftPressed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT);
    bool ctrlPressed = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS;

    if (shiftPressed && ctrlPressed) {
        scalar *= 0.01;
    }
    else if (shiftPressed) {
        scalar *= 0.1;
    }
    else if (ctrlPressed) {
        scalar *= 2;
    }

    switch (text.GetSelection()) {
    case ValueSelection::MirrorRadius:
        config->setMirrorRadius(CLAMP(config->getMirrorRadius() + 0.005f * scalar, 0.1f, 0.5f));
        break;
    case ValueSelection::ProjHorizDistance:
        config->setProjectorHorizontalDistance(CLAMP(config->getProjectorHorizontalDistance() + 0.02f * scalar, 0.4f, 2.0f));
        break;
    case ValueSelection::BottomRowPixels:
        // TODO: BUG!!! Chugs when negative. Setting to only positive right now.
        config->setBottomRowOfPixels(CLAMP(config->getBottomRowOfPixels() + 0.2f * scalar, 0.1f, 10.0f));
        break;
    case ValueSelection::OblateSpheroidAxialRatio:
        config->setOblateSpheroidAxialRatio(CLAMP(config->getOblateSpheroidAxialRatio() + 0.001f * scalar, 0.9f, 0.999999f));
        break;
    case ValueSelection::MirrorCenterToDomeCenter:
        config->setMirrorCenterToDome(CLAMP(config->getMirrorCenterToDomeCenter() + 0.02f * scalar, 1.0f, 3.0f));
        break;
    case ValueSelection::GapDistance:
        config->setGapDistance(CLAMP(config->getGapDistance() + 0.02f * scalar, 0.0f, 2.0f));
        break;
    default:
        break;
    }
}

void processInput(GLFWwindow* window, int key, int scancode, int action, int mods) {
    switch (key) {
    case GLFW_KEY_H:
        if (action == GLFW_RELEASE) {
            hideText = !hideText;
        }
        break;
    case GLFW_KEY_ESCAPE:
        if (action == GLFW_RELEASE) {
            glfwSetWindowShouldClose(window, true);
        }
        break;
    case GLFW_KEY_F11:
        if (action == GLFW_RELEASE) {
            fullscreen = !fullscreen;
            remakeWindow = true;
        }
        break;
    case GLFW_KEY_R:
        if (action == GLFW_RELEASE) {
            text.SetSelection(ValueSelection::MirrorRadius);
        }
        break;
    case GLFW_KEY_V:
        if (action == GLFW_RELEASE) {
            text.SetSelection(ValueSelection::ProjHorizDistance);
        }
        break;
    case GLFW_KEY_A:
        if (action == GLFW_RELEASE) {
            text.SetSelection(ValueSelection::BottomRowPixels);
        }
        break;
    case GLFW_KEY_B:
        if (action == GLFW_RELEASE) {
            text.SetSelection(ValueSelection::OblateSpheroidAxialRatio);
        }
        break;
    case GLFW_KEY_M:
        if (action == GLFW_RELEASE) {
            text.SetSelection(ValueSelection::MirrorCenterToDomeCenter);
        }
        break;
    case GLFW_KEY_G:
        if (action == GLFW_RELEASE) {
            text.SetSelection(ValueSelection::GapDistance);
        }
        break;
    case GLFW_KEY_DOWN:
        if (action == GLFW_RELEASE || action == GLFW_REPEAT) {
            text.SetSelection(static_cast<ValueSelection>((static_cast<int>(text.GetSelection()) + 1) % static_cast<int>(ValueSelection::COUNT)));
        }
        break;
    case GLFW_KEY_UP:
        if (action == GLFW_RELEASE || action == GLFW_REPEAT) {
            text.SetSelection(static_cast<ValueSelection>(((static_cast<int>(text.GetSelection()) - 1) + static_cast<int>(ValueSelection::COUNT)) % static_cast<int>(ValueSelection::COUNT)));
        }
        break;
    case GLFW_KEY_RIGHT:
        if (action == GLFW_RELEASE || action == GLFW_REPEAT) {
            nudge(true);
        }
        break;
    case GLFW_KEY_LEFT:
        if (action == GLFW_RELEASE || action == GLFW_REPEAT) {
            nudge(false);
        }
        break;
    case GLFW_KEY_S:
        if (action == GLFW_PRESS && (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS)) {
            config->saveToFile(configFilename);
        }
    case GLFW_KEY_E:
        if (action == GLFW_PRESS && (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS)) {
            exportData();
        }
    default:
        break;
    }
}

bool makeWindow(bool fullScreen) {
    if (window) {
        auto win = window;
        window = nullptr;
        text.Teardown();
        warper.Teardown();
        glfwDestroyWindow(win);
    }

    if (fullScreen) {
        const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        int monitorWidth = mode->width;
        int monitorHeight = mode->height;
        window = glfwCreateWindow(monitorWidth, monitorHeight, "Planetarium Warper", glfwGetPrimaryMonitor(), nullptr);
    }
    else {
        window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Planetarium Warper", nullptr, nullptr);
    }

    
    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        processInput(window, key, scancode, action, mods);
    });

    warper.Setup();
    text.Setup();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    glViewport(0, 0, width, height);
    text.SetScreenSize(width, height);

    return true;
}

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // configure program
    // -----------------
    config = std::make_shared<ConfigFile>();
    config->loadFromFile(configFilename);
    warper.SetConfig(config);
    text.SetConfig(config);

    if (!makeWindow(fullscreen)) {
        return -1;
    }

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        if (remakeWindow) {
            remakeWindow = false;
            if (!makeWindow(fullscreen)) {
                return -1;
            }
        }

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        warper.Render();

        if (!hideText) {
            text.Render();
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    text.Teardown();
    warper.Teardown();

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    text.SetScreenSize(width, height);
}