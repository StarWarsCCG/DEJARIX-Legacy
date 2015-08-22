#ifndef MAINWIDGET_HPP
#define MAINWIDGET_HPP

#include "Camera.hpp"
#include "CardActor.hpp"
#include "CardRotationAnimation.hpp"
#include "CardPositionAnimation.hpp"
#include "CardSpecifications.hpp"
#include "DeferredArray.hpp"
#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QOpenGLShader>
#include <QImage>
#include <QPoint>
#include <vector>
#include <unordered_map>
#include <memory>
#include <random>

class MainWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget* parent = 0);
    virtual ~MainWidget();

    void dump();
    virtual void keyPressEvent(QKeyEvent* event);

protected slots:
    void onTimer();

protected:
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();

    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void wheelEvent(QWheelEvent* event);

private:
    QOpenGLTexture& loadImage(const QImage& image);
    QOpenGLTexture& loadText(const QString& text);
    GLuint loadMesh(const std::vector<GLfloat>& data);
    GLuint loadIndexBuffer(const std::vector<GLushort>& data);
    void loadCardMesh();

    GLint _viewport[4];
    QMatrix4x4 _projectionMatrix;
    RotationF _fovy;
    Camera _camera;
    QMatrix4x4 _viewMatrices[2];
    bool _isCameraRotating;
    bool _isCameraPanning;
    QPoint _mouse;

    DeferredArray<QOpenGLTexture, 6> _textures;
    std::vector<GLuint> _vertexBufferObjects;
    std::unordered_map<int, CardActor> _cardActors;
    std::vector<CardRotationAnimation> _cardFlipAnimations;
    std::vector<CardRotationAnimation> _cardRotationAnimations;
    std::vector<CardPositionAnimation> _cardPositionAnimations;
    std::vector<CardPositionAnimation> _cardPositionBoomerangs;
    std::mt19937_64 _mt;

    struct
    {
        GLuint position;
        GLuint texture;
    } _attributes;

    struct
    {
        GLuint matrix;
        GLuint texture;
        GLuint highlight;
        GLuint enableTexture;
    } _uniforms;

    struct
    {
        CardSpecifications specifications;
        GLuint vertexBufferObject;
        GLuint indexBufferObject;
        GLsizei topCount;
        GLsizei middleCount;
        GLsizei bottomCount;
        GLushort* middleOffset;
        GLushort* bottomOffset;
    } _cardBuffer;

    struct
    {
        GLuint texture;
    } _tableBuffer;

    QOpenGLShaderProgram _program;
};

#endif
