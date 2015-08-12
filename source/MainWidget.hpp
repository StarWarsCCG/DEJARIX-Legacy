#ifndef MAINWIDGET_HPP
#define MAINWIDGET_HPP

#include "Camera.hpp"
#include "CardDrawTool.hpp"
#include "TableBuffer.hpp"
#include "BasicProgram.hpp"
#include "CardFlipAnimation.hpp"
#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QImage>
#include <QPoint>
#include <vector>
#include <memory>

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
    std::unique_ptr<QOpenGLTexture> loadImage(const QImage& image);
    std::unique_ptr<QOpenGLTexture> loadText(const QString& text);
    QVector3D unproject(QPoint pixel);

    std::unique_ptr<BasicProgram> _program;
    std::unique_ptr<CardBuffer> _cardBuffer;
    std::unique_ptr<CardDrawTool> _drawTool;
    std::unique_ptr<TableBuffer> _tableBuffer;

    GLint _viewport[4];
    QMatrix4x4 _projectionMatrix;
    std::unique_ptr<QOpenGLTexture> _tableTexture;
    Camera _camera;
    QMatrix4x4 _viewMatrix;
    bool _isCameraRotating;
    bool _isCameraPanning;
    QPoint _mouse;

    std::unique_ptr<QOpenGLTexture> _textures[2];
    std::vector<CardActor> _cardActors;
    std::vector<CardFlipAnimation> _cardRotationAnimations;
};

#endif
