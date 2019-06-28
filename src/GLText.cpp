#include "GLText.h"

#include "MathUtils.h"
#include "ShaderUtils.h"
#include "StringUtils.h"
#include <glad/glad.h>

static bool sLibInitialized = false;
static FT_Library sLib;

#define SELECTION(enumVal) if (mSelection == (enumVal)) { mShader->setVec4("color", 0.3f, 1.0f, 0.3f, 1.0f); } else { mShader->setVec4("color", 1.0f, 1.0f, 0.5f, 1.0f); }

void GLText::Setup()
{
    if (!sLibInitialized) {
        if (FT_Init_FreeType(&sLib)) {
            std::cout << "Could not initialize freetype library" << std::endl;
            return;
        }

        sLibInitialized = true;
    }

    if (FT_New_Face(sLib, "Roboto-Regular.ttf", 0, &mFace)) {
        std::cout << "Could not open font" << std::endl;
        return;
    }

    FT_Set_Pixel_Sizes(mFace, 0, 12);

    std::string textVertexCode = readShaderFile("Text.vs");
    std::string textFragCode = readShaderFile("Text.fs");
    mShader = std::unique_ptr<Shader>(new Shader(textVertexCode.c_str(), textFragCode.c_str()));

    glGenVertexArrays(1, &mVAO);

    glBindVertexArray(mVAO);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glGenTextures(1, &mTexture);
    glBindTexture(GL_TEXTURE_2D, mTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

void GLText::Teardown()
{
    glDeleteVertexArrays(1, &mVAO);
    glDeleteBuffers(1, &mVBO);
}

void GLText::Render()
{
    if (!mConfig || !mShader) {
        return;
    }

    glBindTexture(GL_TEXTURE_2D, mTexture);

    glBindVertexArray(mVAO);
    mShader->use();
    float sx = 2.0f / mScreenWidth;
    float sy = 2.0f / mScreenHeight;

    float x = -1 + 8 * sx;
    float y = 1 - 50 * sy;
    float rowAdvance = -15 * sy;

    mShader->setVec4("color", 1.0f, 1.0f, 0.5f, 1.0f);
    renderText("VALUES:", x, y, sx, sy, mFace);
    y += rowAdvance;

    SELECTION(ValueSelection::MirrorRadius);
    renderText("[R] Mirror Radius: " + asString(mConfig->getMirrorRadius()) + "m", x, y, sx, sy, mFace);
    y += rowAdvance;

    SELECTION(ValueSelection::ProjHorizDistance);
    renderText("[V] Proj. Horiz. Distance:  " + asString(mConfig->getProjectorHorizontalDistance()) + "m", x, y, sx, sy, mFace);
    y += rowAdvance;

    SELECTION(ValueSelection::BottomRowPixels);
    renderText("[A] Bottom row of pixels angle:  " + asString(mConfig->getBottomRowOfPixels()) + "deg", x, y, sx, sy, mFace);
    y += rowAdvance;

    SELECTION(ValueSelection::OblateSpheroidAxialRatio);
    renderText("[B] Oblate spheroid axial ratio:  " + asString(mConfig->getOblateSpheroidAxialRatio()), x, y, sx, sy, mFace);
    y += rowAdvance;

    SELECTION(ValueSelection::MirrorCenterToDomeCenter);
    renderText("[M] Mirror center to dome center:  " + asString(mConfig->getMirrorCenterToDomeCenter()) + "m", x, y, sx, sy, mFace);
    y += rowAdvance;

    SELECTION(ValueSelection::GapDistance);
    renderText("[G] Gap distance:  " + asString(mConfig->getGapDistance()) + "m", x, y, sx, sy, mFace);
    y += 2 * rowAdvance;

    mShader->setVec4("color", 1.0f, 1.0f, 0.5f, 1.0f);
    renderText("CONTROLS:", x, y, sx, sy, mFace);
    y += rowAdvance;
    renderText("[ESC] - Quit", x, y, sx, sy, mFace);
    y += rowAdvance;
    renderText("[F11] - Toggle Fullscreen", x, y, sx, sy, mFace);
    y += rowAdvance;
    renderText("[H] - Hide/Show text", x, y, sx, sy, mFace);
    y += rowAdvance;
    renderText("[UP/DOWN] - Select Next/Prev. Value", x, y, sx, sy, mFace);
    y += rowAdvance;
    renderText("[RIGHT/LEFT] - Adjust Value", x, y, sx, sy, mFace);
    y += rowAdvance;
    renderText("[SHIFT, CTRL, or CTRL + SHIFT] - Adjustment Speed Modifiers", x, y, sx, sy, mFace);
    y += rowAdvance;
    renderText("[R, V, A, B, M, or G] - Select Value", x, y, sx, sy, mFace);
    y += rowAdvance;
    renderText("[CTRL] + [S] - Save Config", x, y, sx, sy, mFace);
    y += rowAdvance;
    renderText("[CTRL] + [E] - Export XYUV Data", x, y, sx, sy, mFace);
}

void GLText::renderText(const std::string & txt, float x, float y, float sx, float sy, FT_Face& face)
{
    const char *p;

    const char *text = txt.c_str();

    for (p = text; *p; p++) {
        if (FT_Load_Char(face, *p, FT_LOAD_RENDER))
            continue;

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );

        float x2 = x + face->glyph->bitmap_left * sx;
        float y2 = -y - face->glyph->bitmap_top * sy;
        float w = face->glyph->bitmap.width * sx;
        float h = face->glyph->bitmap.rows * sy;

        GLfloat box[4][4] = {
            {x2,     -y2    , 0, 0},
            {x2 + w, -y2    , 1, 0},
            {x2,     -y2 - h, 0, 1},
            {x2 + w, -y2 - h, 1, 1},
        };

        glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        x += (face->glyph->advance.x / 64) * sx;
        y += (face->glyph->advance.y / 64) * sy;
    }
}

void GLText::SetConfig(std::shared_ptr<ConfigFile> config)
{
    mConfig = config;
}

void GLText::SetScreenSize(int width, int height)
{
    mScreenWidth = width;
    mScreenHeight = height;
}

ValueSelection GLText::GetSelection() const
{
    return mSelection;
}

void GLText::SetSelection(ValueSelection selection)
{
    mSelection = selection;
}
