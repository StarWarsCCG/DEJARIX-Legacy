#include "MainWidget.hpp"
#include "MenuRing.hpp"
#include <QDebug>
#include <QMouseEvent>
#include <QTimer>
#include <QPainter>
#include <QVector2D>

MainWidget::MainWidget(QWidget* parent)
    : QGLWidget(parent)
    , _program(nullptr)
    , _cardBuffer(nullptr)
    , _drawTool(nullptr)
    , _tableBuffer(nullptr)
    , _isCameraRotating(false)
    , _isCameraPanning(false)
{
    _camera.distance(32.0f);
    _camera.angle(RotationF::fromDegrees(-10.0f));
    setMouseTracking(true);
}

MainWidget::~MainWidget()
{
    _program->close();

    deleteTexture(_textures[1]);
    deleteTexture(_textures[0]);
    deleteTexture(_tableTexture);
    delete _drawTool;
    delete _tableBuffer;
    delete _cardBuffer;
    delete _program;
}

void MainWidget::initializeGL()
{
    _functions.initializeOpenGLFunctions();

    QTimer* timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    timer->start(16);

    _program = new BasicProgram(_functions);

    _tableTexture = loadImage(QImage("../wood.jpg"));
    //_tableTexture = loadText("DEJARIX");

    _textures[0] = loadImage(QImage("../localuprising.gif"));
    _textures[1] = loadImage(QImage("../liberation.gif"));

    CardSpecifications specifications;
    CardBuilder builder(specifications);
    _cardBuffer = new CardBuffer(builder);
    _drawTool = new CardDrawTool(*_program, *_cardBuffer, _projectionMatrix);
    _tableBuffer = new TableBuffer(_functions);

    float locationSpan = _cardBuffer->specifications().height()
        + 1.0f / 8.0f;

    for (int i = 0; i < 6; ++i)
    {
        CardActor actor;
        actor.topTexture(_textures[0]);
        actor.bottomTexture(_textures[1]);

        actor.rotation(RotationF::fromDegrees(90.0f));

        _cardActors.append(actor);
    }

    float count = float(_cardActors.size() - 1);
    float firstX = count * locationSpan / -2.0f;

    for (int i = 0; i < _cardActors.size(); ++i)
    {
        _cardActors[i].position(
            QVector3D(
                firstX + float(i) * locationSpan,
                0.0f,
                specifications.depth() / 2.0f));
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.2f, 0.0f, 0.0f, 1.0f);

    _program->open();
}

void MainWidget::resizeGL(int w, int h)
{
    float ratio = float(w) / float(h);
    _projectionMatrix.setToIdentity();
    _projectionMatrix.perspective(60.0f, ratio, 1.0f, 1024.0f);
    glViewport(0, 0, w, h);
    glGetIntegerv(GL_VIEWPORT, _viewport);
}

void MainWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _drawTool->bind();

    for (int i = 0; i < _cardActors.size(); ++i)
    {
        _drawTool->draw(_cardActors[i]);
    }

    _program->enableTexture(true);
    _program->setMatrix(_projectionMatrix * _viewMatrix);
    _program->setHighlight(QVector4D());
    glBindTexture(GL_TEXTURE_2D, _tableTexture);
    _tableBuffer->draw(*_program);
}

void MainWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton)
    {
        _isCameraRotating = true;
        _isCameraPanning = false;
        _mouse = event->pos();
    }
    else if (event->button() == Qt::LeftButton)
    {
        _isCameraPanning = true;
        _isCameraRotating = false;
        _mouse = event->pos();
    }
}

void MainWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton)
        _isCameraRotating = false;
    else if (event->button() == Qt::LeftButton)
        _isCameraPanning = false;
}

void MainWidget::mouseMoveEvent(QMouseEvent* event)
{
    QVector3D mousePosition = unproject(event->pos());
    (void)mousePosition;

    if (_isCameraRotating)
    {
        QPoint delta = event->pos() - _mouse;

        _camera.adjustRotation(RotationF::fromDegrees(float(delta.x()) / 3.0f));
        _camera.adjustAngle(RotationF::fromDegrees(float(delta.y()) / 3.0f));

        _mouse = event->pos();
    }
}

void MainWidget::wheelEvent(QWheelEvent* event)
{
    const float Delta = 3.0f;
    _camera.adjustDistance(event->delta() > 0 ? -Delta : Delta);
}

void MainWidget::onTimer()
{
    _viewMatrix.setToIdentity();
    _camera.apply(_viewMatrix);

    for (auto& actor : _cardActors)
    {
        actor.update(_viewMatrix);
    }

    updateGL();
}

GLuint MainWidget::loadImage(const QImage& image)
{
    GLuint result = 0;

    if (image.width() > 0 && image.height() > 0)
    {
        int width = 1;
        int height = 1;

        while (width < image.width()) width *= 2;
        while (height < image.height()) height *= 2;

        if (width > image.width() || height > image.height())
        {
            result = bindTexture(image.scaled(width, height));
        }
        else
        {
            result = bindTexture(image);
        }

        _functions.glGenerateMipmap(GL_TEXTURE_2D);
    }

    return result;
}

GLuint MainWidget::loadText(const QString& text)
{
    GLuint result = 0;

    QFont font("../DejaVuSans.ttf");
    font.setPixelSize(64);
    QImage image(256, 256, QImage::Format_ARGB32);
    image.fill(QColor(0, 0, 0, 0));

    QPainter painter(&image);
    painter.setFont(font);
    painter.setPen(Qt::green);
    painter.drawText(image.rect(), Qt::AlignCenter, text);
    result = loadImage(image);

    return result;
}

QVector3D MainWidget::unproject(QPoint pixel)
{
    int x = pixel.x();
    int y = height() - pixel.y();

    GLfloat depthSample;
    glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depthSample);

    QVector4D v(
        float(x - _viewport[0]) * 2.0f / float(_viewport[2]) - 1.0f,
        float(y - _viewport[1]) * 2.0f / float(_viewport[3]) - 1.0f,
        2.0f * depthSample - 1.0f,
        1.0f);

    QMatrix4x4 modelViewProjectionMatrix = _projectionMatrix
        * _viewMatrix;
    QMatrix4x4 inverse = modelViewProjectionMatrix.inverted();
    return (inverse * v).toVector3DAffine();
}

void MainWidget::dump()
{
}
