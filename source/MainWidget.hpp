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
    virtual void keyPressEvent(QKeyEvent* event) override;

protected slots:
    void onTimer();

protected:
    virtual void initializeGL() override;
    virtual void resizeGL(int w, int h) override;
    virtual void paintGL() override;

    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void wheelEvent(QWheelEvent* event) override;

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
    bool _isBlackAndWhite;
    QPoint _mouse;

    DeferredArray<QOpenGLTexture, 6> _textures;
    std::vector<GLuint> _bufferObjects;
    std::unordered_map<int, CardActor> _cardActors;
    std::vector<CardRotationAnimation> _cardFlipAnimations;
    std::vector<CardRotationAnimation> _cardRotationAnimations;
    std::vector<CardPositionAnimation> _cardPositionAnimations;
    std::vector<CardPositionAnimation> _cardPositionBoomerangs;
    std::mt19937_64 _mt;

    struct
    {
        int position;
        int texture;
    } _attributes;

    struct
    {
        int matrix;
        int colorMatrix;
        int texture;
        int highlight;
        int enableTexture;
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
    } _cardModel;

    struct
    {
        GLuint texture;
        GLfloat mesh[20];
    } _tableModel;

    QOpenGLShaderProgram _program;
};

#endif
