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
    , _isCameraRotating(false)
    , _isCameraPanning(false)
    , _isBlackAndWhite(false)
{
    _fovy = RotationF::fromDegrees(60.0f);
    _camera.distance = 32.0f;
    _camera.angle = RotationF::fromDegrees(-10.0f);

    // Actual card dimensions! 63mm x 88mm
    _cardModel.specifications = {6.3f, 8.8f, 0.05f, 0.25f, 4};

    _colorMatrixAnimation.stepCount = 15;
    _colorMatrixAnimation.currentStep = 30;

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

    for (auto i : _cardActors)
    {
        auto actor = i.second;
        _program.setUniformValue(
            _uniforms.matrix, _projectionMatrix * actor.modelViewMatrix);
        _program.setUniformValue(_uniforms.highlight, actor.highlight);
        _program.setUniformValue(_uniforms.enableTexture, false);

        glDrawElements(
            GL_TRIANGLES,
            _cardModel.middleCount,
            GL_UNSIGNED_SHORT,
            _cardModel.middleOffset);

        _program.setUniformValue(_uniforms.enableTexture, true);

        if (actor.isTopVisible)
        {
            glBindTexture(GL_TEXTURE_2D, actor.topTexture);
            glDrawElements(
                GL_TRIANGLES,
                _cardModel.topCount,
                GL_UNSIGNED_SHORT,
                nullptr);
        }
        else
        {
            glBindTexture(GL_TEXTURE_2D, actor.bottomTexture);
            glDrawElements(
                GL_TRIANGLES,
                _cardModel.bottomCount,
                GL_UNSIGNED_SHORT,
                _cardModel.bottomOffset);
        }
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
    for (int i = 0; i < 60; ++i)
    {
        auto z = _cardModel.specifications.depth / 2.0f * float(i * 2 + 1);

        CardActor actor;
        actor.topTexture = _textures[1].textureId();
        actor.bottomTexture = _textures[2].textureId();
        actor.position = QVector3D(0.0f, 0.0f, z);

        //actor.rotation = RotationF::fromDegrees(90.0f);

        _cardActors[i] = actor;
    }

    for (int i = 0; i < 60; ++i)
    {
        auto z = _cardModel.specifications.depth / 2.0f * float(i * 2 + 1);

        CardActor actor;
        actor.topTexture = _textures[1].textureId();
        actor.bottomTexture = _textures[2].textureId();
        actor.position = QVector3D(8.0f, 0.0f, z);

        _cardActors[i + 60] = actor;
    }

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
        for (int i = 0; i < 60; ++i)
        {
            auto actor = _cardActors[i];
            auto rotation = actor.rotation.toRadians();
            _cardRotationAnimations.push_back(
                {i, rotation, rotation + pi<float>(), 0.25f, 75 - i, 0});
        }

        break;
    }

    case Qt::Key_W:
    {
        std::uniform_int_distribution<int> distribution(15, 45);
        for (int i = 0; i < 120; ++i)
        {
            auto actor = _cardActors[i];
            CardPositionAnimation cpa;
            cpa.cardId = i;
            cpa.control.points[0] = actor.position;
            cpa.control.points[1] =
                actor.position + QVector3D(2.5f, 1.0f, 0.0f);
            cpa.control.points[2] =
                actor.position + QVector3D(5.0f, 0.0f, 0.0f);
            cpa.stepCount = distribution(_mt);
            cpa.currentStep = 0;

            _cardPositionAnimations.push_back(cpa);
        }
        break;
    }

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
        std::uniform_int_distribution<int> distribution(0, 59);
        int n = distribution(_mt);

        QVector3D bounce(
            0.0f, 0.0f, _cardModel.specifications.width);
        auto actor = _cardActors[n];

        CardPositionAnimation cpa;
        cpa.cardId = n;
        cpa.control.points[0] = actor.position;
        cpa.control.points[1] = actor.position + bounce;
        cpa.control.points[2] = actor.position;
        cpa.stepCount = 30;
        cpa.currentStep = 0;

        _cardPositionAnimations.push_back(cpa);

        auto flip = actor.flip.toRadians();
        _cardFlipAnimations.push_back(
            {n, flip, flip + pi<float>(), 0.0f, 30, 0});

        for (int i = n + 1; i < 60; ++i)
        {
            actor = _cardActors[i];
            cpa.cardId = i;
            cpa.control.points[0] = actor.position;
            cpa.control.points[1] = actor.position + bounce * 2.0f;
            cpa.control.points[2] = actor.position;

            _cardPositionAnimations.push_back(cpa);
        }

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
        cpa.control.points[2] = QVector3D(-12.0f, 0.0f, _cardModel.specifications.depth / 2.0f);
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
        if (_stateChanged)
            qDebug() << "state changed";
        else
            qDebug() << "state unchanged";

        _stateChangeLog = _stateChanged;
    }

    if (_stateChanged)
    {
        _viewMatrices[CameraMatrixIndex].setToIdentity();
        _camera.apply(_viewMatrices[CameraMatrixIndex]);

        for (auto& i : _cardActors) i.second.update(_viewMatrices);

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

void MainWidget::dump()
{
    qDebug() << QDir::homePath();
}
