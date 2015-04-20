#ifndef CARDBUFFER_HPP
#define CARDBUFFER_HPP

#include "CardBuilder.hpp"
#include <QOpenGLFunctions>

class CardBuffer : protected QOpenGLFunctions
{
public:
    CardBuffer(const CardBuilder& builder);
    virtual ~CardBuffer();

    inline const CardSpecifications& specifications() const
    {
        return _specifications;
    }

    void bind(const BasicProgram& program);
    void drawTop();
    void drawMiddle();
    void drawBottom();

private:
    CardSpecifications _specifications;
    BasicBufferObject _object;
    GLuint _indexBuffer;
    GLsizei _topCount;
    GLsizei _middleCount;
    GLsizei _bottomCount;
    GLushort* _middleOffset;
    GLushort* _bottomOffset;
};

#endif
