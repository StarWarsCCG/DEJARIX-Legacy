#include "CardActor.hpp"

CardActor::CardActor()
    : _topTexture(0)
    , _bottomTexture(0)
    , _isTopVisible(true)
    , _depthFactor(1.0f)
{
}

void CardActor::update(const QMatrix4x4& viewMatrix)
{
    _modelMatrix.setToIdentity();
    _modelMatrix.translate(_position);
    _modelMatrix.rotate(_flip.toDegrees(), 0.0f, 1.0f, 0.0f);
    _modelMatrix.rotate(_rotation.toDegrees(), 0.0f, 0.0f, 1.0f);
    _modelMatrix.scale(1.0f, 1.0f, _depthFactor);

    _modelViewMatrix = viewMatrix * _modelMatrix;

    QVector4D origin(0.0f, 0.0f, 0.0f, 1.0f);
    QVector4D modelViewOrigin = _modelViewMatrix * origin;

    QVector4D arrow(0.0f, 0.0f, 1.0f, 1.0f);
    QVector4D modelViewArrow = _modelViewMatrix * arrow;

    QVector3D direction = modelViewArrow.toVector3DAffine()
        - modelViewOrigin.toVector3DAffine();
    QVector3D cameraToPolygon = QVector3D(modelViewOrigin).normalized();
    float dotProduct = QVector3D::dotProduct(cameraToPolygon, direction);
    _isTopVisible = dotProduct < 0.0f;
}
