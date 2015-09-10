#ifndef CARDACTOR_HPP
#define CARDACTOR_HPP

#include "Rotation.hpp"
#include <QVector3D>
#include <QMatrix4x4>
#include <QOpenGLFunctions>

constexpr int AirMatrixIndex = 0;
constexpr int CameraMatrixIndex = 1;

struct CardActor
{
    GLuint topTexture = 0;
    GLuint bottomTexture = 0;
    float depthFactor = 1.0f;
    int viewMatrixIndex = CameraMatrixIndex;

    QVector4D highlight;
    QVector3D position;
    RotationF rotation;
    RotationF flip;
};

#endif
