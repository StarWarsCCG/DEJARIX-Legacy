#include "Camera.hpp"
#include <cmath>

Camera::Camera() : _distance(0.0f)
{
}

Camera::~Camera()
{
}

void Camera::apply(QMatrix4x4& matrix)
{
    matrix.translate(0.0f, 0.0f, -_distance);
    matrix.rotate(_angle.toDegrees(), 1.0f, 0.0f, 0.0f);
    matrix.rotate(_rotation.toDegrees(), 0.0f, 0.0f, 1.0f);
    matrix.translate(-_position);
}

void Camera::panRelative(float x, float y)
{
    float theta = _rotation.toRadians();
    float c = cos(theta);
    float s = sin(theta);

    float deltaX = c * x;
    float deltaY = -s * x;
    deltaX += s * y;
    deltaY += c * y;

    _position.setX(_position.x() + deltaX);
    _position.setY(_position.y() + deltaY);
}
