#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "Rotation.hpp"
#include <QVector3D>
#include <QMatrix4x4>
#include <QVector2D>
#include <QtMath>

struct Camera
{
    void panRelative(QVector2D delta);
    void apply(QMatrix4x4& matrix) const;
    void applyOpposite(QMatrix4x4& matrix) const;

    QVector3D position;
    float distance = 0.0f;
    RotationF rotation;
    RotationF angle;
};

#endif
