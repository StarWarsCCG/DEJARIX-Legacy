#include "MainWidget.hpp"
#include <QDebug>
#include <QMouseEvent>
#include <QTimer>
#include <QPainter>
#include <QVector2D>
#include <QDir>

static constexpr float TableV = 512.0f;
static constexpr float TableT = 128.0f;

static constexpr GLsizei Stride = sizeof(GLfloat) * 5;


static float findDistance(RotationF fov, float height)
{
    auto theta = fov.toRadians() / 2.0f;
    auto h = height / 2.0f;
    return h / tan(theta);
}

MainWidget::MainWidget(QWidget* parent)
    : QOpenGLWidget(parent)
{
    Q_UNUSED(findDistance);
    _fovy = RotationF::fromDegrees(60.0f);
    _camera.distance = 32.0f;
    _camera.angle = RotationF::fromDegrees(-10.0f);

    // Actual card dimensions! 63mm x 88mm
    _cardModel.specifications = {6.3f, 8.8f, 0.05f, 0.25f, 4};

    _colorMatrixAnimation.stepCount = 15;
    _colorMatrixAnimation.currentStep = 30;

    float w = _cardModel.specifications.width + 1.0f;
    float h = _cardModel.specifications.height + 1.0f;

    _relativePileLocations.reserveDeck = QVector2D(0.0f, 0.0f);
    _relativePileLocations.forcePile = QVector2D(w, 0.0f);
    _relativePileLocations.usedPile = QVector2D(0.0f, h);
    _relativePileLocations.lostPile = QVector2D(-w, 0.0f);
    _relativePileLocations.outOfPlay = QVector2D(w * -2.0f, 0.0f);

    setDarkLocations({0.0f, -16.0f});
    setLightLocations({0.0f, 16.0f});

    setMouseTracking(true);
}

MainWidget::~MainWidget()
{
    makeCurrent();
    _program.release();

    glDeleteBuffers(_bufferObjects.size(), _bufferObjects.data());
}

void MainWidget::initializeGL()
{
    initializeOpenGLFunctions();

    QTimer* timer = new QTimer(this);
    timer->setTimerType(Qt::PreciseTimer);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    timer->start(16);

    _tableModel.texture = loadImage(QImage("../wood.jpg")).textureId();
    loadImage(QImage("../localuprising.gif"));
    loadImage(QImage("../liberation.gif"));

    loadCardMesh();

    constexpr GLfloat TableMesh[] = {
        TableV, TableV, 0.0f, TableT, TableT,
        TableV, -TableV, 0.0f, TableT, -TableT,
        -TableV, -TableV, 0.0f, -TableT, -TableT,
        -TableV, TableV, 0.0f, -TableT, TableT
    };

    memcpy(_tableModel.mesh, TableMesh, sizeof(_tableModel.mesh));

    constexpr auto VertexShaderSource =
        "attribute highp vec4 position;\n"
        "attribute lowp vec2 tc;\n"
        "varying lowp vec2 vtc;\n"
        "uniform highp mat4 matrix;\n"
        "void main() {\n"
        "   vtc = tc;\n"
        "   gl_Position = matrix * position;\n"
        "}\n";

    constexpr auto FragmentShaderSource =
#ifdef Q_OS_WIN
        // This produces a warning in Linux:
        // warning C7022: unrecognized profile specifier "precision"

        // This explodes in OSX. Apparently, only Windows demands it.
        //"precision highp float;\n"
#endif
        "uniform bool enableTexture;\n"
        "uniform highp mat4 colorMatrix;\n"
        "uniform sampler2D texture;\n"
        "uniform lowp vec4 highlight;\n"
        "varying lowp vec2 vtc;\n"
        "void main() {\n"
        "   vec4 result = vec4(0.0, 0.0, 0.0, 1.0);\n"
        "   if (enableTexture) result = texture2D(texture, vtc);\n"
        "   gl_FragColor = colorMatrix * (result + highlight);\n"
        "}\n";

    _program.addShaderFromSourceCode(
        QOpenGLShader::Vertex,
        VertexShaderSource);
    _program.addShaderFromSourceCode(
        QOpenGLShader::Fragment,
        FragmentShaderSource);

    _program.link();
    _program.bind();
    _attributes.position = _program.attributeLocation("position");
    _attributes.texture = _program.attributeLocation("tc");
    _uniforms.matrix = _program.uniformLocation("matrix");
    _uniforms.colorMatrix = _program.uniformLocation("colorMatrix");
    _uniforms.texture = _program.uniformLocation("texture");
    _uniforms.highlight = _program.uniformLocation("highlight");
    _uniforms.enableTexture = _program.uniformLocation("enableTexture");
    _program.setUniformValue(_uniforms.texture, 0);
    _program.setUniformValue(_uniforms.colorMatrix, QMatrix4x4());
    _program.setUniformValue(_uniforms.enableTexture, true);
    _program.setUniformValue(_uniforms.highlight, QVector4D());

    resetCards();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.2f, 0.0f, 0.0f, 1.0f);

    glEnableVertexAttribArray(_attributes.position);
    glEnableVertexAttribArray(_attributes.texture);
}

void MainWidget::resizeGL(int w, int h)
{
    _stateChanged = true;
    float ratio = float(w) / float(h);
    _projectionMatrix.setToIdentity();
    _projectionMatrix.perspective(_fovy.toDegrees(), ratio, 1.0f, 1024.0f);
    glViewport(0, 0, w, h);
    glGetIntegerv(GL_VIEWPORT, _viewport);

    onTimer();
}

void MainWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindBuffer(GL_ARRAY_BUFFER, _cardModel.vertexBufferObject);

    glVertexAttribPointer(
        _attributes.position,
        3,
        GL_FLOAT,
        GL_FALSE,
        Stride,
        nullptr);

    glVertexAttribPointer(
        _attributes.texture,
        2,
        GL_FLOAT,
        GL_FALSE,
        Stride,
        (GLfloat*)0 + 3);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _cardModel.indexBufferObject);
    _program.setUniformValue(_uniforms.colorMatrix, _colorMatrix);

    for (const auto& render : _faceUpCards)
    {
        _program.setUniformValue(_uniforms.matrix, render.matrix);
        _program.setUniformValue(_uniforms.highlight, render.highlight);
        _program.setUniformValue(_uniforms.enableTexture, false);

        glDrawElements(
            GL_TRIANGLES,
            _cardModel.middleCount,
            GL_UNSIGNED_SHORT,
            _cardModel.middleOffset);

        _program.setUniformValue(_uniforms.enableTexture, true);

        glBindTexture(GL_TEXTURE_2D, render.texture);
        glDrawElements(
            GL_TRIANGLES,
            _cardModel.topCount,
            GL_UNSIGNED_SHORT,
            nullptr);
    }

    for (const auto& render : _faceDownCards)
    {
        _program.setUniformValue(_uniforms.matrix, render.matrix);
        _program.setUniformValue(_uniforms.highlight, render.highlight);
        _program.setUniformValue(_uniforms.enableTexture, false);

        glDrawElements(
            GL_TRIANGLES,
            _cardModel.middleCount,
            GL_UNSIGNED_SHORT,
            _cardModel.middleOffset);

        _program.setUniformValue(_uniforms.enableTexture, true);

        glBindTexture(GL_TEXTURE_2D, render.texture);
        glDrawElements(
            GL_TRIANGLES,
            _cardModel.bottomCount,
            GL_UNSIGNED_SHORT,
            _cardModel.bottomOffset);
    }

    _program.setUniformValue(_uniforms.enableTexture, true);
    _program.setUniformValue(
        _uniforms.matrix,
        _projectionMatrix * _viewMatrices[CameraMatrixIndex]);
    _program.setUniformValue(_uniforms.highlight, QVector4D());

    glBindTexture(GL_TEXTURE_2D, _tableModel.texture);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glVertexAttribPointer(
        _attributes.position,
        3,
        GL_FLOAT,
        GL_FALSE,
        Stride,
        _tableModel.mesh);

    glVertexAttribPointer(
        _attributes.texture,
        2,
        GL_FLOAT,
        GL_FALSE,
        Stride,
        _tableModel.mesh + 3);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
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
    if (_isCameraRotating)
    {
        _stateChanged = true;
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
    _stateChanged = true;
}

void MainWidget::resetCards()
{
    qint64 cardInfoIds[60];
    for (auto& cardInfoId : cardInfoIds) cardInfoId = 1;
    _state.start({cardInfoIds, 60, 0}, {cardInfoIds, 60, 0});

    _pileCounts[0].reserveDeck = 60;
    _pileCounts[0].forcePile = 0;
    _pileCounts[0].usedPile = 0;
    _pileCounts[0].lostPile = 0;
    _pileCounts[0].outOfPlay = 0;

    _pileCounts[1].reserveDeck = 60;
    _pileCounts[1].forcePile = 0;
    _pileCounts[1].usedPile = 0;
    _pileCounts[1].lostPile = 0;
    _pileCounts[1].outOfPlay = 0;

    _cardActors.clear();

    for (int i = 0; i < 60; ++i)
    {
        auto z = _cardModel.specifications.depth / 2.0f * float(i * 2 + 1);

        CardActor actor;
        actor.topTexture = _textures[1].textureId();
        actor.bottomTexture = _textures[2].textureId();
        actor.position = QVector3D(_pileLocations[0].reserveDeck, z);

        _cardActors[i] = actor;
    }

    for (int i = 0; i < 60; ++i)
    {
        auto z = _cardModel.specifications.depth / 2.0f * float(i * 2 + 1);

        CardActor actor;
        actor.topTexture = _textures[1].textureId();
        actor.bottomTexture = _textures[2].textureId();
        actor.position = QVector3D(_pileLocations[1].reserveDeck, z);
        actor.rotation = RotationF::half();

        _cardActors[60 + i] = actor;
    }

    _cardFlipAnimations.clear();
    _cardRotationAnimations.clear();
    _cardPositionAnimations.clear();

    _stateChanged = true;
}

void MainWidget::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
    case Qt::Key_F5:
        resetCards();
        break;

    case Qt::Key_F6:
    {
        _isBlackAndWhite = !_isBlackAndWhite;

        QMatrix4x4 blackAndWhite(
            0.3f, 0.6f, 0.1f, 0.0f,
            0.3f, 0.6f, 0.1f, 0.0f,
            0.3f, 0.6f, 0.1f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f);

        constexpr auto S = 0.5f;
        blackAndWhite.scale(S, S, S);

        _colorMatrixAnimation.currentStep = 0;

        if (_isBlackAndWhite)
        {
            _colorMatrixAnimation.firstMatrix = QMatrix4x4();
            _colorMatrixAnimation.lastMatrix = blackAndWhite;
        }
        else
        {
            _colorMatrixAnimation.firstMatrix = blackAndWhite;
            _colorMatrixAnimation.lastMatrix = QMatrix4x4();
        }

        break;
    }

    case Qt::Key_Q:
    {
        setDarkLocations(QVector2D(2.0f, 2.0f));

        for (int i = 0; i < _state.darkSideCount; ++i)
        {
            CardState cs = _state.cardStateByInstanceId[i];
            if (cs.mode == CollectionMode &&
                cs.location >= DarkSide(ReserveDeck) &&
                cs.location <= DarkSide(OutOfPlay))
            {
                CardActor cardActor = _cardActors[i];

                CardPositionAnimation cpa;
                cpa.cardId = i;
                cpa.stepCount = 30;
                cpa.currentStep = 0;
                cpa.control.points[0] = cardActor.position;

                QVector2D pilePosition;
                switch (cs.location)
                {
                case DarkSide(ReserveDeck):
                    pilePosition = _pileLocations[0].reserveDeck;
                    break;
                case DarkSide(ForcePile):
                    pilePosition = _pileLocations[0].forcePile;
                    break;
                case DarkSide(UsedPile):
                    pilePosition = _pileLocations[0].usedPile;
                    break;
                case DarkSide(LostPile):
                    pilePosition = _pileLocations[0].lostPile;
                    break;
                case DarkSide(OutOfPlay):
                    pilePosition = _pileLocations[0].outOfPlay;
                    break;
                default:
                    qDebug() << Q_FUNC_INFO << "How did we get here?";
                    break;
                }

                cpa.control.points[2] =
                    QVector3D(pilePosition, cardActor.position.z());
                cpa.control.points[1] =
                    (cpa.control.points[0] + cpa.control.points[2]) / 2.0f;

                _cardPositionAnimations.push_back(cpa);
            }
        }

        break;
    }

    case Qt::Key_W:
    {
        quint8 cardId = _state.topCard(DarkSide(ForcePile));
        qDebug() << cardId;
        if (cardId == 255) break;
        CardState cardState;
        cardState.mode = CollectionMode;
        cardState.location = DarkSide(UsedPile);
        cardState.ordinal = _pileCounts[0].usedPile;
        cardState.rotation = 0;
        cardState.isFaceUp = false;
        _state.cardStateByInstanceId[cardId] = cardState;

        CardActor cardActor = _cardActors[cardId];
        float depth = _cardModel.specifications.depth;

        auto usedPileCount = float(_pileCounts[0].usedPile);

        CardPositionAnimation cpa;
        cpa.cardId = cardId;
        cpa.currentStep = 0;
        cpa.stepCount = 60;
        cpa.control.points[0] = cardActor.position;
        cpa.control.points[2] = QVector3D(
            _pileLocations[0].usedPile, depth / 2.0f +
            depth * usedPileCount);
        cpa.control.points[1] =
            (cpa.control.points[2] + cpa.control.points[0]) / 2.0f +
            QVector3D(4.0f, 0.0f, usedPileCount * 0.125f + 2.0f);

        _cardPositionAnimations.push_back(cpa);
        ++_pileCounts[0].usedPile;
        --_pileCounts[0].forcePile;

        emit cardEvent("used force");

        break;
    }

    case Qt::Key_A:
    {
        quint8 cardId = _state.topCard(DarkSide(ReserveDeck));
        qDebug() << cardId;
        if (cardId == 255) break;
        CardState cardState;
        cardState.mode = CollectionMode;
        cardState.location = DarkSide(LostPile);
        cardState.ordinal = _pileCounts[0].lostPile;
        cardState.rotation = 0;
        cardState.isFaceUp = true;
        _state.cardStateByInstanceId[cardId] = cardState;

        CardActor cardActor = _cardActors[cardId];
        float depth = _cardModel.specifications.depth;

        auto lostPileCount = float(_pileCounts[0].lostPile);

        CardPositionAnimation cpa;
        cpa.cardId = cardId;
        cpa.currentStep = 0;
        cpa.stepCount = 60;
        cpa.control.points[0] = cardActor.position;
        cpa.control.points[2] = QVector3D(
            _pileLocations[0].lostPile,
            depth / 2.0f + depth * lostPileCount);
        cpa.control.points[1] =
            (cpa.control.points[2] + cpa.control.points[0]) / 2.0f +
            QVector3D(-4.0f, 0.0f, lostPileCount * 0.125f + 12.0f);

        _cardPositionAnimations.push_back(cpa);

        CardRotationAnimation cra;
        cra.cardId = cardId;
        cra.currentStep = 0;
        cra.stepCount = 30;
        cra.firstRadians = cardActor.rotation.toRadians();
        cra.lastRadians = cra.firstRadians - pi<float>();

        _cardFlipAnimations.push_back(cra);

        ++_pileCounts[0].lostPile;
        --_pileCounts[0].reserveDeck;

        emit cardEvent("lost card from Reserve Deck");
        break;
    }

    case Qt::Key_S:
    {
        if (_pileCounts[0].usedPile < 1) break;

        auto usedPileSize = float(_pileCounts[0].usedPile) *
            _cardModel.specifications.depth;

        for (int i = 0; i < _state.darkSideCount; ++i)
        {
            CardState& cardState = _state.cardStateByInstanceId[i];

            if (cardState.mode != CollectionMode) continue;

            if (cardState.location == DarkSide(ReserveDeck))
            {
                CardActor cardActor = _cardActors[i];

                CardPositionAnimation cpa;
                cpa.cardId = i;
                cpa.stepCount = 30;
                cpa.currentStep = 0;

                _cardPositionAnimations.push_back(cpa);
                cpa.control.points[0] = cardActor.position;
                cpa.control.points[2] =
                    cardActor.position + QVector3D(0.0f, 0.0f, usedPileSize);
                cpa.control.points[1] =
                    cpa.control.points[2] + QVector3D(0.0f, 0.0f, usedPileSize);

                _cardPositionAnimations.push_back(cpa);
                cardState.ordinal += _pileCounts[0].usedPile;
            }
            else if (cardState.location == DarkSide(UsedPile))
            {
                CardActor cardActor = _cardActors[i];

                CardPositionAnimation cpa;
                cpa.cardId = i;
                cpa.stepCount = 30;
                cpa.currentStep = 0;

                cpa.control.points[0] = cardActor.position;
                cpa.control.points[2] =
                    QVector3D(
                        _pileLocations[0].reserveDeck,
                        cardActor.position.z());
                cpa.control.points[1] =
                    (cpa.control.points[0] + cpa.control.points[2]) / 2.0f;

                _cardPositionAnimations.push_back(cpa);
                cardState.location = DarkSide(ReserveDeck);
            }
        }

        _pileCounts[0].reserveDeck += _pileCounts[0].usedPile;
        _pileCounts[0].usedPile = 0;

        emit cardEvent("cycled Used Pile under Reserve Deck");

        break;
    }

    case Qt::Key_D:
    {
        int n = 0;
        quint8 cardId = _state.topCard(DarkSide(ReserveDeck));

        if (event->modifiers() & Qt::ShiftModifier)
        {
            n = 1;
            cardId = _state.topCard(LightSide(ReserveDeck));
        }

        qDebug() << cardId;
        if (cardId == 255) break;
        CardState cardState;
        cardState.mode = CollectionMode;
        cardState.location = AutoSide(ForcePile, n);
        cardState.ordinal = _pileCounts[n].forcePile;
        cardState.rotation = 0;
        cardState.isFaceUp = false;
        _state.cardStateByInstanceId[cardId] = cardState;

        CardActor cardActor = _cardActors[cardId];
        float depth = _cardModel.specifications.depth;

        auto forcePileCount = float(_pileCounts[n].forcePile);

        CardPositionAnimation cpa;
        cpa.cardId = cardId;
        cpa.currentStep = 0;
        cpa.stepCount = 60;
        cpa.control.points[0] = cardActor.position;
        cpa.control.points[2] = QVector3D(
            _pileLocations[n].forcePile,
            depth / 2.0f + depth * forcePileCount);

        float swing = n ? -4.0f : 4.0f;
        cpa.control.points[1] =
            (cpa.control.points[2] + cpa.control.points[0]) / 2.0f +
            QVector3D(swing, 0.0f, forcePileCount * 0.125f + 2.0f);

        _cardPositionAnimations.push_back(cpa);
        ++_pileCounts[n].forcePile;
        --_pileCounts[n].reserveDeck;

        emit cardEvent("activated force");

        break;
    }

    case Qt::Key_F:
    {
        auto actor = _cardActors[2];
        auto lastPosition = QVector3D(
            actor.position.x(),
            16.0f - actor.position.y(),
            actor.position.z());

        CardPositionAnimation cpa;
        cpa.cardId = 2;
        cpa.control.points[0] = actor.position;
        cpa.control.points[1] = Linear(actor.position, lastPosition, 1.25f);
        cpa.control.points[2] = lastPosition;
        cpa.stepCount = 30;
        cpa.currentStep = 0;

        _cardPositionAnimations.push_back(cpa);

        break;
    }

    case Qt::Key_G:
    {
        auto actor = _cardActors[59];

        CardPositionAnimation cpa;
        cpa.cardId = 59;
        cpa.control.points[0] = actor.position;
        cpa.control.points[1] = QVector3D(-6.0f, 4.0f, 8.0f);
        cpa.control.points[2] =
            QVector3D(-12.0f, 0.0f, _cardModel.specifications.depth / 2.0f);
        cpa.stepCount = 15;
        cpa.currentStep = 0;

        _cardPositionAnimations.push_back(cpa);
        break;
    }

    default:
        break;
    }
}

void MainWidget::onTimer()
{
    if (_colorMatrixAnimation.currentStep <= _colorMatrixAnimation.stepCount)
    {
        _stateChanged = true;
        auto t = float(_colorMatrixAnimation.currentStep++) /
            float(_colorMatrixAnimation.stepCount);

        _colorMatrix = Linear(
            _colorMatrixAnimation.firstMatrix,
            _colorMatrixAnimation.lastMatrix,
            t);
    }

    for (auto i = _cardRotationAnimations.begin();
         i != _cardRotationAnimations.end();)
    {
        _stateChanged = true;
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
        _stateChanged = true;
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
        _stateChanged = true;
        auto& actor = _cardActors[i->cardId];

        if (i->currentStep < i->stepCount)
        {
            auto t = float(i->currentStep++) / float(i->stepCount);
            //actor.position = Linear(i->firstPosition, i->lastPosition, t);
            actor.position = Interpolate(i->control, t);
            //actor.position = SCurve(i->firstPosition, i->lastPosition, t);

            ++i;
        }
        else
        {
            actor.position = i->control.points[2];
            i = _cardPositionAnimations.erase(i);
        }
    }

    if (_stateChangeLog != _stateChanged)
    {
//        if (_stateChanged)
//            qDebug() << "state changed";
//        else
//            qDebug() << "state unchanged";

        _stateChangeLog = _stateChanged;
    }

    if (_stateChanged)
    {
        _viewMatrices[CameraMatrixIndex].setToIdentity();
        _camera.apply(_viewMatrices[CameraMatrixIndex]);

        _faceUpCards.clear();
        _faceDownCards.clear();

        for (const auto& pair : _cardActors)
        {
            const CardActor& actor = pair.second;
            QMatrix4x4 modelMatrix;
            modelMatrix.translate(actor.position);
            modelMatrix.rotate(actor.flip.toDegrees(), 0.0f, 1.0f, 0.0f);
            modelMatrix.rotate(actor.rotation.toDegrees(), 0.0f, 0.0f, 1.0f);
            modelMatrix.scale(1.0f, 1.0f, actor.depthFactor);

            QMatrix4x4 modelViewMatrix =
                _viewMatrices[actor.viewMatrixIndex] * modelMatrix;

            constexpr QVector4D Origin(0.0f, 0.0f, 0.0f, 1.0f);
            constexpr QVector4D Arrow(0.0f, 0.0f, 1.0f, 1.0f);

            QVector4D modelViewOrigin = modelViewMatrix * Origin;
            QVector4D modelViewArrow = modelViewMatrix * Arrow;
            QVector3D direction =
                modelViewArrow.toVector3DAffine() -
                modelViewOrigin.toVector3DAffine();
            QVector3D cameraToPolygon =
                QVector3D(modelViewOrigin).normalized();
            float dotProduct =
                QVector3D::dotProduct(cameraToPolygon, direction);

            CardRender render;
            render.matrix = _projectionMatrix * modelViewMatrix;
            render.highlight = actor.highlight;

            if (dotProduct < 0.0f)
            {
                render.texture = actor.topTexture;
                _faceUpCards.push_back(render);
            }
            else
            {
                render.texture = actor.bottomTexture;
                _faceDownCards.push_back(render);
            }
        }

        update();
        _stateChanged = false;
    }
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

GLuint MainWidget::loadMesh(const std::vector<GLfloat>& data)
{
    GLuint object;
    glGenBuffers(1, &object);
    glBindBuffer(GL_ARRAY_BUFFER, object);
    glBufferData(
        GL_ARRAY_BUFFER,
        data.size() * sizeof(GLfloat),
        data.data(),
        GL_STATIC_DRAW);

    _bufferObjects.push_back(object);
    return object;
}

GLuint MainWidget::loadIndexBuffer(const std::vector<GLushort>& data)
{
    GLuint object;
    glGenBuffers(1, &object);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        data.size() * sizeof(GLushort),
        data.data(),
        GL_STATIC_DRAW);

    _bufferObjects.push_back(object);
    return object;
}

void MainWidget::loadCardMesh()
{
    std::vector<GLfloat> vertices;
    std::vector<GLfloat> textureCoordinates;
    std::vector<GLushort> topIndices;
    std::vector<GLushort> middleIndices;
    std::vector<GLushort> bottomIndices;

    auto addVertex = [&](float x, float y, float s, float t)
    {
        float z = _cardModel.specifications.depth / 2.0f;

        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(z);
        textureCoordinates.push_back(s);
        textureCoordinates.push_back(t);

        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(-z);
        textureCoordinates.push_back(1.0f - s);
        textureCoordinates.push_back(t);
    };

    auto addTriangle = [&](GLushort a, GLushort b, GLushort c)
    {
        middleIndices.push_back(a);
        middleIndices.push_back(b);
        middleIndices.push_back(c);
    };

    auto addTriangles = [&](GLushort a, GLushort b, GLushort c)
    {
        topIndices.push_back(a);
        topIndices.push_back(b);
        topIndices.push_back(c);

        bottomIndices.push_back(c + 1);
        bottomIndices.push_back(b + 1);
        bottomIndices.push_back(a + 1);
    };

    auto addQuad = [&](GLushort a, GLushort b, GLushort c, GLushort d)
    {
        addTriangle(a, b, c);
        addTriangle(a, c, d);
    };

    auto addQuads = [&](GLushort a, GLushort b, GLushort c, GLushort d)
    {
        addTriangles(a, b, c);
        addTriangles(a, c, d);
    };

    auto specifications = _cardModel.specifications;

    float w = specifications.width / 2.0f;
    float h = specifications.height / 2.0f;
    float cr = specifications.cornerRadius;
    int cd = specifications.cornerDetail;

    int vertexCount = 8 * cd + 16;
    vertices.reserve(vertexCount * 3);
    textureCoordinates.reserve(vertexCount * 2);

    // upper left corner
    addVertex(cr - w, h - cr, cr / specifications.width,
        cr / specifications.height);

    addVertex(-w, h - cr, 0.0f, cr / specifications.height);

    float theta = 90.0f / float(cd);
    for (int i = 1; i < cd; ++i)
    {
        float radians = float(i) * theta * radiansPerDegree<float>();
        float dx = cr * cos(radians);
        float dy = cr * sin(radians);

        addVertex(
            cr - w - dx,
            h  - cr + dy,
            (cr - dx) / specifications.width,
            (cr - dy) / specifications.height);
    }

    addVertex(cr - w, h, cr / specifications.width, 0.0f);

    // upper right corner
    addVertex(-vertices[0], vertices[1],
        1.0f - textureCoordinates[0],
        textureCoordinates[1]);

    int previousVertex = vertices.size() - 6;
    int previousTexture = textureCoordinates.size() - 4;

    int mirrorVertex = vertices.size() - 12;
    int mirrorTexture = textureCoordinates.size() - 8;

    for (int i = 0; i <= cd; ++i)
    {
        addVertex(
            -vertices[mirrorVertex],
            vertices[mirrorVertex + 1],
            1.0f - textureCoordinates[mirrorTexture],
            textureCoordinates[mirrorTexture + 1]);

        mirrorVertex -= 6;
        mirrorTexture -= 4;
    }

    // lower right corner
    addVertex(
        vertices[previousVertex],
        -vertices[previousVertex + 1],
        textureCoordinates[previousTexture],
        1.0f - textureCoordinates[previousTexture + 1]);

    previousVertex = vertices.size() - 6;
    previousTexture = textureCoordinates.size() - 4;

    mirrorVertex = vertices.size() - 12;
    mirrorTexture = textureCoordinates.size() - 8;

    for (int i = 0; i <= cd; ++i)
    {
        addVertex(
            vertices[mirrorVertex],
            -vertices[mirrorVertex + 1],
            textureCoordinates[mirrorTexture],
            1.0f - textureCoordinates[mirrorTexture + 1]);

        mirrorVertex -= 6;
        mirrorTexture -= 4;
    }

    // lower left corner
    addVertex(
        -vertices[previousVertex],
        vertices[previousVertex + 1],
        1.0f - textureCoordinates[previousTexture],
        textureCoordinates[previousTexture + 1]);

    previousVertex = vertices.size() - 6;
    previousTexture = textureCoordinates.size() - 4;

    mirrorVertex = vertices.size() - 12;
    mirrorTexture = textureCoordinates.size() - 8;

    for (int i = 0; i <= cd; ++i)
    {
        addVertex(
            -vertices[mirrorVertex],
            vertices[mirrorVertex + 1],
            1.0f - textureCoordinates[mirrorTexture],
            textureCoordinates[mirrorTexture + 1]);

        mirrorVertex -= 6;
        mirrorTexture -= 4;
    }
    // texture correction
    for (int i = 0; i < vertexCount; ++i)
    {
        int index = i * 2 + 1;
        textureCoordinates[index] = 1.0f - textureCoordinates[index];
    }

    // index creation
    int triangleCount = 16 * cd + 28;
    int middleTriangleCount = 8 * cd + 8;
    int topTriangleCount = (triangleCount - middleTriangleCount) / 2;

    topIndices.reserve(topTriangleCount * 3);
    middleIndices.reserve(middleTriangleCount * 3);
    bottomIndices.reserve(topTriangleCount * 3);

    GLushort corners[5];
    int cornerSize = 2 * cd + 4;

    for (int i = 0; i < 5; ++i)
        corners[i] = cornerSize * i;

    for (int i = 0; i < 4; ++i)
    {
        int i4 = (i + 1) % 4;
        addQuads(
            corners[i],
            corners[i + 1] - 2,
            corners[i4] + 2,
            corners[i4]);

        addQuad(
            corners[i + 1] - 2,
            corners[i + 1] - 1,
            corners[i4] + 3,
            corners[i4] + 2);

        for (int j = 0; j < cd; ++j)
        {
            int k = 2 * (j + 1);
            addTriangles(
                corners[i],
                corners[i] + k,
                corners[i] + k + 2);

            addQuad(
                corners[i] + k,
                corners[i] + k + 1,
                corners[i] + k + 3,
                corners[i] + k + 2);
        }
    }

    addQuads(corners[0], corners[1], corners[2], corners[3]);

    std::vector<GLfloat> mesh;
    mesh.reserve(vertexCount * 5);

    for (int i = 0; i < vertexCount; ++i)
    {
        int vi = i * 3;
        mesh.push_back(vertices[vi + 0]);
        mesh.push_back(vertices[vi + 1]);
        mesh.push_back(vertices[vi + 2]);

        int ti = i * 2;
        mesh.push_back(textureCoordinates[ti + 0]);
        mesh.push_back(textureCoordinates[ti + 1]);
    }

    std::vector<GLushort> indices;
    indices.reserve(
        topIndices.size() + middleIndices.size() + bottomIndices.size());

    for (auto index : topIndices) indices.push_back(index);
    for (auto index : middleIndices) indices.push_back(index);
    for (auto index : bottomIndices) indices.push_back(index);

    _cardModel.vertexBufferObject = loadMesh(mesh);
    _cardModel.indexBufferObject = loadIndexBuffer(indices);
    _cardModel.topCount = topIndices.size();
    _cardModel.middleCount = middleIndices.size();
    _cardModel.bottomCount = bottomIndices.size();
    _cardModel.middleOffset = (GLushort*)0 + topIndices.size();
    _cardModel.bottomOffset = _cardModel.middleOffset + middleIndices.size();
}

void MainWidget::setDarkLocations(QVector2D position)
{
    _pileLocations[0].reserveDeck = position;
    _pileLocations[0].forcePile =
        _pileLocations[0].reserveDeck + _relativePileLocations.forcePile;
    _pileLocations[0].usedPile =
        _pileLocations[0].reserveDeck + _relativePileLocations.usedPile;
    _pileLocations[0].lostPile =
        _pileLocations[0].reserveDeck + _relativePileLocations.lostPile;
    _pileLocations[0].outOfPlay =
        _pileLocations[0].reserveDeck + _relativePileLocations.outOfPlay;
}

void MainWidget::setLightLocations(QVector2D position)
{
    _pileLocations[1].reserveDeck = position;
    _pileLocations[1].forcePile =
        _pileLocations[1].reserveDeck - _relativePileLocations.forcePile;
    _pileLocations[1].usedPile =
        _pileLocations[1].reserveDeck - _relativePileLocations.usedPile;
    _pileLocations[1].lostPile =
        _pileLocations[1].reserveDeck - _relativePileLocations.lostPile;
    _pileLocations[1].outOfPlay =
        _pileLocations[1].reserveDeck - _relativePileLocations.outOfPlay;
}

void MainWidget::dump()
{
    qDebug() << QDir::homePath();
}
