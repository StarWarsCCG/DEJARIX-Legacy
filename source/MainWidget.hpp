#ifndef MAINWIDGET_HPP
#define MAINWIDGET_HPP

#include "Camera.hpp"
#include "CardActor.hpp"
#include "CardRotationAnimation.hpp"
#include "CardPositionAnimation.hpp"
#include "CardSpecifications.hpp"
#include "MatrixAnimation.hpp"
#include "DeferredArray.hpp"
#include "Piles.hpp"
#include "GameState.hpp"
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

struct CardRender
{
    QMatrix4x4 matrix;
    QVector4D highlight;
    GLuint texture;
};

struct CardMatrixSwap
{
    int cardId;
    int stepsRemaining;
};

class MainWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget* parent = nullptr);
    virtual ~MainWidget();

    void dump();
    virtual void keyPressEvent(QKeyEvent* event) override;

protected slots:
    void onTimer();

signals:
    void cardEvent(QString description);

protected:
    virtual void initializeGL() override;
    virtual void resizeGL(int w, int h) override;
    virtual void paintGL() override;

    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void wheelEvent(QWheelEvent* event) override;

private:
    void prepareRender();
    void resetCards();
    QOpenGLTexture& loadImage(const QImage& image);
    QOpenGLTexture& loadText(const QString& text);
    GLuint loadMesh(const std::vector<GLfloat>& data);
    GLuint loadIndexBuffer(const std::vector<GLushort>& data);
    void loadCardMesh();
    void setDarkLocations(QVector2D position);
    void setLightLocations(QVector2D position);

    GLint _viewport[4];
    QMatrix4x4 _projectionMatrix;
    RotationF _fovy;
    Camera _camera;
    QMatrix4x4 _viewMatrices[2];
    QMatrix4x4 _colorMatrix;
    bool _isCameraRotating = false;
    bool _isCameraPanning = false;
    bool _isBlackAndWhite = false;
    bool _stateChanged = true;
    bool _stateChangeLog = true;
    QPoint _mouse;
    float _darkSideReserve = 0.0f;

    DeferredArray<QOpenGLTexture, 6> _textures;
    std::vector<GLuint> _bufferObjects;
    std::unordered_map<int, CardActor> _cardActorsById;
    std::vector<CardRender> _faceUpCards;
    std::vector<CardRender> _faceDownCards;
    std::vector<CardRotationAnimation> _cardFlipAnimations;
    std::vector<CardRotationAnimation> _cardRotationAnimations;
    std::vector<CardPositionAnimation> _cardPositionAnimations;
    std::vector<CardMatrixSwap> _cardMatrixSwaps;
    MatrixAnimation _colorMatrixAnimation;
    std::mt19937_64 _mt;
    PileSet<QVector2D> _relativePileLocations;
    PileSet<QVector2D> _pileLocations[2];
    PileSet<int> _pileCounts[2];
    GameState _state;

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
