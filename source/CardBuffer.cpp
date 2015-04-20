#include "CardBuffer.hpp"
#include <QDebug>

CardBuffer::CardBuffer(const CardBuilder& builder)
    : _specifications(builder.specifications())
    , _middleOffset(nullptr)
    , _bottomOffset(nullptr)
{
    initializeOpenGLFunctions();
    _object = builder.bufferObject(*this);

    glGenBuffers(1, &_indexBuffer);

    _topCount = builder.topIndices().size();
    _middleCount = builder.middleIndices().size();
    _bottomCount = builder.bottomIndices().size();

    QVector<GLushort> indices;
    indices.reserve(_topCount + _middleCount + _bottomCount);
    indices += builder.topIndices();
    indices += builder.middleIndices();
    indices += builder.bottomIndices();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(GLushort),
        indices.constData(),
        GL_STATIC_DRAW);

    _middleOffset = (GLushort*)0 + _topCount;
    _bottomOffset = _middleOffset + _middleCount;
}

CardBuffer::~CardBuffer()
{
    glDeleteBuffers(1, &_indexBuffer);
}

void CardBuffer::drawTop()
{
    glDrawElements(
        GL_TRIANGLES,
        _topCount,
        GL_UNSIGNED_SHORT,
        0);
}

void CardBuffer::drawMiddle()
{
    glDrawElements(
        GL_TRIANGLES,
        _middleCount,
        GL_UNSIGNED_SHORT,
        _middleOffset);
}

void CardBuffer::drawBottom()
{
    glDrawElements(
        GL_TRIANGLES,
        _bottomCount,
        GL_UNSIGNED_SHORT,
        _bottomOffset);
}

void CardBuffer::bind(const BasicProgram& program)
{
    _object.bind(program);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);
}
