#pragma once

#include <string>

class ConfigFile {
public:
    void loadFromFile(const std::string& filename);

    // Initialization-only constants
    float getDomeRadius() const;
    float getPhaseAngle() const;
    bool getFlipX() const;
    bool getFlipY() const;
    float getProjectorOpeningAngle() const;
    float getAspectRatio() const;
    float getTableTilt() const;

    // Runtime variable
    float getMirrorCenterToDomeCenter() const;
    void setMirrorCenterToDome(float val);

    float getGapDistance() const;
    void setGapDistance(float val);

    float getMirrorRadius() const;
    void setMirrorRadius(float val);

    float getProjectorHorizontalDistance() const;
    void setProjectorHorizontalDistance(float val);

    float getBottomRowOfPixels() const;
    void setBottomRowOfPixels(float val);

    float getOblateSpheroidAxialRatio() const;
    void setOblateSpheroidAxialRatio(float val);

private:
    float mDomeRadius;
    float mPhaseAngle;
    bool mFlipX;
    bool mFlipY;
    float mProjectorOpeningAngle;
    float mAspectRatio;
    float mTableTilt;
    float mMirrorCenterToDomeCenter;
    float mGapDistance;
    float mMirrorRadius;
    float mProjectorHorizontalDistance;
    float mBottomRowOfPixels;
    float mOblateSpheroidAxialRatio;
};