#include "GLWarper.h"
#include "ShaderUtils.h"
#include "StringUtils.h"
#include "MathUtils.h"
#include <engine/shader.h>
#include <stb_image.h>

namespace {
    void replaceAll(std::string& str, const std::string& from, const std::string& to) {
        if (from.empty())
            return;
        size_t start_pos = 0;
        while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
        }
    }
}

void GLWarper::Setup()
{
    if (!mConfig) {
        return;
    }

    // read shader code
    // ----------------
    std::string vertexCode = readShaderFile("FullscreenQuad.vs.in");
    std::string fragmentCode = readShaderFile("FullscreenQuad.fs.in");

    // Shader file constant variable replacements
    // ------------------------------------------
    replaceAll(vertexCode, "$(UV_SCALE_X)", mConfig->getFlipX() ? "-1.0" : "1.0");
    replaceAll(vertexCode, "$(UV_SCALE_Y)", mConfig->getFlipY() ? "-1.0" : "1.0");
    replaceAll(vertexCode, "$(UV_OFFSET_X)", mConfig->getFlipX() ? "1.0" : "0.0");
    replaceAll(vertexCode, "$(UV_OFFSET_Y)", mConfig->getFlipY() ? "1.0" : "0.0");

    replaceAll(fragmentCode, "$(DOME_RADIUS)", asString(mConfig->getDomeRadius()));
    replaceAll(fragmentCode, "$(PHASE_ANGLE)", asString(mConfig->getPhaseAngle()));
    replaceAll(fragmentCode, "$(BEAM)", asString(mConfig->getProjectorOpeningAngle()));
    replaceAll(fragmentCode, "$(ASPECT)", asString(mConfig->getAspectRatio()));
    replaceAll(fragmentCode, "$(ALPHA)", asString(mConfig->getTableTilt()));

    // build and compile our shader program
    // ------------------------------------
    mShader = std::unique_ptr<Shader>(new Shader(vertexCode.c_str(), fragmentCode.c_str()));

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions         // texture coords
         1.0f,  1.0f, 0.0f,  1.0f, 1.0f, // top right
         1.0f, -1.0f, 0.0f,  1.0f, 0.0f, // bottom right
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, // bottom left
        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f  // top left 
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &mVBO);
    glGenBuffers(1, &mEBO);

    glBindVertexArray(mVAO);

    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // load and create textures 
    // -------------------------
    glGenTextures(1, &mTexture);
    glBindTexture(GL_TEXTURE_2D, mTexture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    unsigned char *data = stbi_load("radialgrid.tga", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
}

void GLWarper::Teardown()
{
    glDeleteVertexArrays(1, &mVAO);
    glDeleteBuffers(1, &mVBO);
    glDeleteBuffers(1, &mEBO);
    mShader.reset();
}

void GLWarper::Render()
{
    if (!mConfig || !mShader) {
        return;
    }

    glBindTexture(GL_TEXTURE_2D, mTexture);

    mShader->use();

    mShader->setFloat("R", mConfig->getMirrorRadius());
    mShader->setFloat("v", mConfig->getProjectorHorizontalDistance());
    mShader->setFloat("beta", degrees2Radians(mConfig->getBottomRowOfPixels()));
    mShader->setFloat("ba", mConfig->getOblateSpheroidAxialRatio());
    mShader->setFloat("M", mConfig->getMirrorCenterToDomeCenter());
    mShader->setFloat("H", mConfig->getGapDistance());
    mShader->setBool("renderWarp", mMode == GLWarperRenderMode::Normal ? false : true);

    glBindVertexArray(mVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void GLWarper::SetConfig(std::shared_ptr<ConfigFile> config)
{
    mConfig = config;
}

void GLWarper::SetRenderMode(GLWarperRenderMode mode)
{
    mMode = mode;
}
