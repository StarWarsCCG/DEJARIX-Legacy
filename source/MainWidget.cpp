#include "MainWidget.hpp"
#include <QDebug>
#include <QMouseEvent>
#include <QTimer>
#include <QPainter>
#include <QVector2D>
#include <QDir>

MainWidget::MainWidget(QWidget* parent)
    : QOpenGLWidget(parent)
    , _isCameraRotating(false)
    , _isCameraPanning(false)
{
    _camera.distance = 32.0f;
    _camera.angle = RotationF::fromDegrees(-10.0f);
    setMouseTracking(true);
}

MainWidget::~MainWidget()
{
    makeCurrent();
    _program->close();
}

void MainWidget::initializeGL()
{
    initializeOpenGLFunctions();

    QTimer* timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    timer->start(16);

    _program = std::unique_ptr<BasicProgram>(new BasicProgram(*this));

    loadImage(QImage("../localuprising.gif"));
    loadImage(QImage("../liberation.gif"));
    loadImage(QImage("../wood.jpg"));

    CardSpecifications specifications;
    CardBuilder builder(specifications);
    _cardBuffer = std::unique_ptr<CardBuffer>(new CardBuffer(builder));
    _drawTool = std::unique_ptr<CardDrawTool>(
        new CardDrawTool(*_program, *_cardBuffer, _projectionMatrix));
    _tableBuffer = std::unique_ptr<TableBuffer>(new TableBuffer(*this));

    float locationSpan = _cardBuffer->specifications().height + 1.0f;

    for (int i = 0; i < 6; ++i)
    {
        CardActor actor;
        actor.topTexture = _textures[0].textureId();
        actor.bottomTexture = _textures[1].textureId();

        //actor.rotation = RotationF::fromDegrees(90.0f);

        _cardActors.push_back(actor);
    }

    float count = float(_cardActors.size() - 1);
    float firstX = count * locationSpan / -2.0f;

    for (std::size_t i = 0; i < _cardActors.size(); ++i)
    {
        _cardActors[i].position =
            QVector3D(
                firstX + float(i) * locationSpan,
                0.0f,
                specifications.depth / 2.0f);
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

    for (const auto& actor : _cardActors) _drawTool->draw(actor);

    _program->enableTexture(true);
    _program->setMatrix(_projectionMatrix * _viewMatrix);
    _program->setHighlight(QVector4D());
    _textures[2].bind();
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

        _camera.rotation += RotationF::fromDegrees(float(delta.x()) / 3.0f);
        _camera.angle += RotationF::fromDegrees(float(delta.y()) / 3.0f);

        _mouse = event->pos();
    }
}

void MainWidget::wheelEvent(QWheelEvent* event)
{
    const float Delta = 3.0f;
    _camera.distance += event->delta() > 0 ? -Delta : Delta;
}

void MainWidget::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
    case Qt::Key_A:
    {
        for (int i = 0; i < 6; ++i)
        {
            auto ii = i + 1;
            const auto& actor = _cardActors[i];
            auto rotation = actor.rotation.toRadians();
            _cardRotationAnimations.push_back(
                { i, rotation, rotation + pi<float>(), 0.125f * ii, 60, 0 });
        }
        break;
    }

    case Qt::Key_S:
    {
        for (int i = 0; i < 6; ++i)
        {
            auto ii = i + 1;
            const auto& actor = _cardActors[i];
            auto rotation = actor.rotation.toRadians();
            _cardRotationAnimations.push_back(
                { i, rotation, rotation - pi<float>(), 0.25f, ii * 15, 0 });
        }
        break;
    }

    case Qt::Key_D:
    {
        for (int i = 0; i < 6; ++i)
        {
            auto ii = i + 1;
            auto& actor = _cardActors[i];
            actor.position.setZ(4.0f);
            auto flip = actor.flip.toRadians();
            _cardFlipAnimations.push_back(
                { i, flip, flip + pi<float>(), 0.25f, ii * 15, 0 });
        }
        break;
    }

    case Qt::Key_F:
    {
        auto& actor = _cardActors[2];
        auto lastPosition = QVector3D(
            actor.position.x(),
            16.0f - actor.position.y(),
            actor.position.z());
        _cardPositionAnimations.push_back(
            { 2, actor.position, lastPosition, 30, 0 });

        break;
    }

    case Qt::Key_G:
    {
        auto& actor = _cardActors[3];
        auto lastPosition = QVector3D(
            actor.position.x() + 2.0f,
            actor.position.y() + 8.0f,
            actor.position.z() + 2.0f);
        _cardPositionBoomerangs.push_back(
            { 3, actor.position, lastPosition, 30, 0 });
        break;
    }

    default:
        break;
    }
}

template<typename T1, typename T2> T1 Linear(T1 first, T1 last, T2 t)
{
    auto difference = last - first;
    return difference * t + first;
}

template<typename T1, typename T2> T1 Overshoot(
    T1 first, T1 last, T2 magnitude, T2 t)
{
    auto difference = last - first;

    return
        difference * t + first - sin(t * tau<T2>()) * magnitude * difference;
}

template<typename T1, typename T2> T1 SCurve(T1 first, T1 last, T2 t)
{
    auto difference = last - first;
    auto tt = (T2(1) - cos(t * pi<T2>())) * T2(0.5);
    return difference * tt + first;
}

template<typename T1, typename T2> T1 Boomerang(T1 first, T1 last, T2 t)
{
    auto tt = sin(t * pi<T2>());
    return Linear(first, last, tt);
}

void MainWidget::onTimer()
{
    for (auto i = _cardRotationAnimations.begin();
         i != _cardRotationAnimations.end();)
    {
        auto& actor = _cardActors[i->cardId];

        if (i->currentStep < i->stepCount)
        {
            auto t = float(i->currentStep++) / float(i->stepCount);
            actor.rotation = RotationF::fromRadians(Overshoot<float>(
                i->firstRadians,
                i->lastRadians,
                i->magnitude,
                t));

            ++i;
        }
        else
        {
            actor.rotation = RotationF::fromRadians(i->lastRadians);
            i = _cardRotationAnimations.erase(i);
        }
    }

    for (auto i = _cardFlipAnimations.begin();
         i != _cardFlipAnimations.end();)
    {
        auto& actor = _cardActors[i->cardId];

        if (i->currentStep < i->stepCount)
        {
            auto t = float(i->currentStep++) / float(i->stepCount);
            actor.flip = RotationF::fromRadians(Overshoot<float>(
                i->firstRadians,
                i->lastRadians,
                i->magnitude,
                t));

            ++i;
        }
        else
        {
            actor.flip = RotationF::fromRadians(i->lastRadians);
            i = _cardFlipAnimations.erase(i);
        }
    }

    for (auto i = _cardPositionAnimations.begin();
         i != _cardPositionAnimations.end();)
    {
        auto& actor = _cardActors[i->cardId];

        if (i->currentStep < i->stepCount)
        {
            auto t = float(i->currentStep++) / float(i->stepCount);
            //actor.position = Linear(i->firstPosition, i->lastPosition, t);
            actor.position = SCurve(i->firstPosition, i->lastPosition, t);

            ++i;
        }
        else
        {
            actor.position = i->lastPosition;
            i = _cardPositionAnimations.erase(i);
        }
    }

    for (auto i = _cardPositionBoomerangs.begin();
         i != _cardPositionBoomerangs.end();)
    {
        auto& actor = _cardActors[i->cardId];

        if (i->currentStep < i->stepCount)
        {
            auto t = float(i->currentStep++) / float(i->stepCount);
            actor.position = Boomerang(i->firstPosition, i->lastPosition, t);

            ++i;
        }
        else
        {
            actor.position = i->firstPosition;
            i = _cardPositionBoomerangs.erase(i);
        }
    }

    _viewMatrix.setToIdentity();
    _camera.apply(_viewMatrix);

    for (auto& actor : _cardActors) actor.update(_viewMatrix);

    update();
}

QOpenGLTexture& MainWidget::loadImage(const QImage& image)
{
    if (image.width() > 0 && image.height() > 0)
    {
        int width = 1;
        int height = 1;

        while (width < image.width()) width *= 2;
        while (height < image.height()) height *= 2;

        if (width > image.width() || height > image.height())
        {
            // http://doc.qt.io/qt-5/qopengltexture.html#details
            // Qt flips the images vertically... on purpose... *sigh*

            auto& texture = _textures.push(
                image.scaled(width, height).mirrored(false, true),
                QOpenGLTexture::MipMapGeneration::GenerateMipMaps);

            texture.setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
            texture.setMagnificationFilter(QOpenGLTexture::Linear);

            return texture;
        }
        else
        {
            auto& texture = _textures.push(
                image.mirrored(false, true),
                QOpenGLTexture::MipMapGeneration::GenerateMipMaps);

            texture.setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
            texture.setMagnificationFilter(QOpenGLTexture::Linear);

            return texture;
        }
    }

    return _textures.push(QOpenGLTexture::Target2D);
}

QOpenGLTexture& MainWidget::loadText(const QString& text)
{
    QFont font("../DejaVuSans.ttf");
    font.setPixelSize(64);
    QImage image(256, 256, QImage::Format_ARGB32);
    image.fill(QColor(0, 0, 0, 0));

    QPainter painter(&image);
    painter.setFont(font);
    painter.setPen(Qt::green);
    painter.drawText(image.rect(), Qt::AlignCenter, text);
    return loadImage(image);
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
    qDebug() << QDir::homePath();
}
