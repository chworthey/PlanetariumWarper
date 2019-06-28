#include "ConfigFile.h"

#include <fstream>
#include <json/json.h>

void ConfigFile::loadFromFile(const std::string &filename)
{
    Json::Value root;
    std::ifstream file(filename);
    file >> root;
    file.close();

    mDomeRadius = root["DomeRadiusMeters"].asFloat();
    mPhaseAngle = root["PhaseAngleDegrees"].asFloat();
    mFlipX = root["FlipX"].asBool();
    mFlipY = root["FlipY"].asBool();
    mProjectorOpeningAngle = root["ProjectorOpeningAngleDegrees"].asFloat();
    mAspectRatio = root["AspectRatio"].asFloat();
    mTableTilt = root["TableTiltDegrees"].asFloat();
    mExportSize = root["ExportSize"].asInt();
    mMirrorCenterToDomeCenter = root["MirrorCenterToDomeCenterMeters"].asFloat();
    mGapDistance = root["GapDistanceMeters"].asFloat();
    mMirrorRadius = root["MirrorRadiusMeters"].asFloat();
    mProjectorHorizontalDistance = root["ProjectorHorizontalDistanceMeters"].asFloat();
    mBottomRowOfPixels = root["BottomRowOfPixelsDegrees"].asFloat();
    mOblateSpheroidAxialRatio = root["OblateSpheroidAxialRatio"].asFloat();
}

void ConfigFile::saveToFile(const std::string& filename)
{
    Json::Value root;
    root["DomeRadiusMeters"] = mDomeRadius;
    root["PhaseAngleDegrees"] = mPhaseAngle;
    root["FlipX"] = mFlipX;
    root["FlipY"] = mFlipY;
    root["ProjectorOpeningAngleDegrees"] = mProjectorOpeningAngle;
    root["AspectRatio"] = mAspectRatio;
    root["TableTiltDegrees"] = mTableTilt;
    root["ExportSize"] = mExportSize;
    root["MirrorCenterToDomeCenterMeters"] = mMirrorCenterToDomeCenter;
    root["GapDistanceMeters"] = mGapDistance;
    root["MirrorRadiusMeters"] = mMirrorRadius;
    root["ProjectorHorizontalDistanceMeters"] = mProjectorHorizontalDistance;
    root["BottomRowOfPixelsDegrees"] = mBottomRowOfPixels;
    root["OblateSpheroidAxialRatio"] = mOblateSpheroidAxialRatio;

    std::ofstream file(filename);
    file << root;
    file.close();
}

float ConfigFile::getMirrorRadius() const
{
    return mMirrorRadius;
}

void ConfigFile::setMirrorRadius(float val)
{
    mMirrorRadius = val;
}

float ConfigFile::getProjectorHorizontalDistance() const
{
    return mProjectorHorizontalDistance;
}

void ConfigFile::setProjectorHorizontalDistance(float val)
{
    mProjectorHorizontalDistance = val;
}

float ConfigFile::getBottomRowOfPixels() const
{
    return mBottomRowOfPixels;
}

void ConfigFile::setBottomRowOfPixels(float val)
{
    mBottomRowOfPixels = val;
}

float ConfigFile::getOblateSpheroidAxialRatio() const
{
    return mOblateSpheroidAxialRatio;
}

void ConfigFile::setOblateSpheroidAxialRatio(float val)
{
    mOblateSpheroidAxialRatio = val;
}

float ConfigFile::getDomeRadius() const
{
    return mDomeRadius;
}

float ConfigFile::getPhaseAngle() const
{
    return mPhaseAngle;
}

bool ConfigFile::getFlipX() const
{
    return mFlipX;
}

bool ConfigFile::getFlipY() const
{
    return mFlipY;
}

float ConfigFile::getProjectorOpeningAngle() const
{
    return mProjectorOpeningAngle;
}

float ConfigFile::getAspectRatio() const
{
    return mAspectRatio;
}

float ConfigFile::getTableTilt() const
{
    return mTableTilt;
}

int ConfigFile::getExportSize() const
{
    return mExportSize;
}

float ConfigFile::getMirrorCenterToDomeCenter() const
{
    return mMirrorCenterToDomeCenter;
}

void ConfigFile::setMirrorCenterToDome(float val)
{
    mMirrorCenterToDomeCenter = val;
}

float ConfigFile::getGapDistance() const
{
    return mGapDistance;
}

void ConfigFile::setGapDistance(float val)
{
    mGapDistance = val;
}
