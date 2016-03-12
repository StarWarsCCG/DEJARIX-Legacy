#include "Camera.hpp"
#include <cmath>

void Camera::apply(QMatrix4x4& matrix) const
{
    matrix.translate(0.0f, 0.0f, -distance);
    matrix.rotate(angle.toDegrees(), 1.0f, 0.0f, 0.0f);
    matrix.rotate(rotation.toDegrees(), 0.0f, 0.0f, 1.0f);
    matrix.translate(-position);
}

void Camera::applyOpposite(QMatrix4x4 &matrix) const
{



    matrix.translate(position);
    matrix.rotate(-rotation.toDegrees(), 0.0f, 0.0f, 1.0f);
    matrix.rotate(-angle.toDegrees(), 1.0f, 0.0f, 0.0f);
    matrix.translate(0.0f, 0.0f, distance);
}

void Camera::panRelative(QVector2D delta)
{
    float theta = rotation.toRadians();
    float c = cos(theta);
    float s = sin(theta);

    float deltaX = c * delta.x();
    float deltaY = -s * delta.x();
    deltaX += s * delta.y();
    deltaY += c * delta.y();

    position.setX(position.x() + deltaX);
    position.setY(position.y() + deltaY);
}
