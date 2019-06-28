#pragma once

#include "ConfigFile.h"

#include <engine/shader.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <memory>
#include <string>

enum class ValueSelection : uint8_t {
    MirrorRadius = 0,
    ProjHorizDistance = 1,
    BottomRowPixels = 2,
    OblateSpheroidAxialRatio = 3,
    MirrorCenterToDomeCenter = 4,
    GapDistance = 5,
    COUNT = 6
};

class GLText {
public:
  void Setup();
  void Teardown();
  void Render();
  void SetConfig(std::shared_ptr<ConfigFile> config);
  void SetScreenSize(int width, int height);
  ValueSelection GetSelection() const;
  void SetSelection(ValueSelection selection);

private:
    void renderText(const std::string &txt, float x, float y, float sx, float sy, FT_Face& face);
    unsigned int mVBO;
    unsigned int mVAO;
    unsigned int mTexture;
    int mScreenWidth;
    int mScreenHeight;
    ValueSelection mSelection = ValueSelection::MirrorRadius;
    FT_Face mFace;
    std::shared_ptr<ConfigFile> mConfig;
    std::unique_ptr<Shader> mShader;
};