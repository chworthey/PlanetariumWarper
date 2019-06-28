#pragma once

#include "ConfigFile.h"
#include "engine/shader.h"
#include <memory>

enum class GLWarperRenderMode : uint8_t {
    Normal = 0,
    Export = 1
};

class GLWarper {
public:
  void Setup();
  void Teardown();
  void Render();
  void SetConfig(std::shared_ptr<ConfigFile> config);
  void SetRenderMode(GLWarperRenderMode mode);

private:
    unsigned int mVBO;
    unsigned int mVAO;
    unsigned int mEBO;
    unsigned int mTexture;
    std::shared_ptr<ConfigFile> mConfig;
    std::unique_ptr<Shader> mShader;
    GLWarperRenderMode mMode = GLWarperRenderMode::Normal;
};