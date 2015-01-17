#ifndef CARDACTOR_HPP
#define CARDACTOR_HPP

#include "Rotation.hpp"
#include <QVector3D>
#include <QMatrix4x4>
#include <QOpenGLFunctions>

struct CardActor
{
    GLuint topTexture = 0;
    GLuint bottomTexture = 0;
    bool isTopVisible = true;
    float depthFactor = 1.0f;

    QVector4D highlight;
    QVector3D position;
    RotationF rotation;
    RotationF flip;

    QMatrix4x4 modelMatrix;
    QMatrix4x4 modelViewMatrix;

    void update(const QMatrix4x4& viewMatrix);
};

#endif
