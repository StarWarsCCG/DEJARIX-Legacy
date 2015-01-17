#include "CardActor.hpp"

void CardActor::update(const QMatrix4x4& viewMatrix)
{
    modelMatrix.setToIdentity();
    modelMatrix.translate(position);
    modelMatrix.rotate(flip.toDegrees(), 0.0f, 1.0f, 0.0f);
    modelMatrix.rotate(rotation.toDegrees(), 0.0f, 0.0f, 1.0f);
    modelMatrix.scale(1.0f, 1.0f, depthFactor);

    modelViewMatrix = viewMatrix * modelMatrix;

    QVector4D origin(0.0f, 0.0f, 0.0f, 1.0f);
    QVector4D modelViewOrigin = modelViewMatrix * origin;

    QVector4D arrow(0.0f, 0.0f, 1.0f, 1.0f);
    QVector4D modelViewArrow = modelViewMatrix * arrow;

    QVector3D direction = modelViewArrow.toVector3DAffine()
        - modelViewOrigin.toVector3DAffine();
    QVector3D cameraToPolygon = QVector3D(modelViewOrigin).normalized();
    float dotProduct = QVector3D::dotProduct(cameraToPolygon, direction);
    isTopVisible = dotProduct < 0.0f;
}
