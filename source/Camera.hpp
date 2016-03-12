#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "Rotation.hpp"
#include <QVector3D>
#include <QMatrix4x4>
#include <QtMath>

struct Camera
{
    void panRelative(float x, float y);
    void apply(QMatrix4x4& matrix) const;

    QVector3D position;
    float distance = 0.0f;
    RotationF rotation;
    RotationF angle;
};

#endif
