#include "CardActor.hpp"

static constexpr QVector4D Origin(0.0f, 0.0f, 0.0f, 1.0f);
static constexpr QVector4D Arrow(0.0f, 0.0f, 1.0f, 1.0f);

void CardActor::update(QMatrix4x4 viewMatrix)
{
    QMatrix4x4 modelMatrix;
    modelMatrix.translate(position);
    modelMatrix.rotate(flip.toDegrees(), 0.0f, 1.0f, 0.0f);
    modelMatrix.rotate(rotation.toDegrees(), 0.0f, 0.0f, 1.0f);
    modelMatrix.scale(1.0f, 1.0f, depthFactor);

    modelViewMatrix = viewMatrix * modelMatrix;

    QVector4D modelViewOrigin = modelViewMatrix * Origin;
    QVector4D modelViewArrow = modelViewMatrix * Arrow;

    QVector3D direction =
        modelViewArrow.toVector3DAffine() - modelViewOrigin.toVector3DAffine();
    QVector3D cameraToPolygon = QVector3D(modelViewOrigin).normalized();
    float dotProduct = QVector3D::dotProduct(cameraToPolygon, direction);
    isTopVisible = dotProduct < 0.0f;
}
