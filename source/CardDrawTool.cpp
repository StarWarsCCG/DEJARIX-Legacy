#include "CardDrawTool.hpp"
#include <QDebug>

CardDrawTool::CardDrawTool(
    BasicProgram& program,
    CardBuffer& buffer,
    QMatrix4x4& projectionMatrix)
    : _program(program),
    _buffer(buffer),
    _projectionMatrix(projectionMatrix)
{
}

CardDrawTool::~CardDrawTool()
{
}

void CardDrawTool::bind()
{
    _buffer.bind(_program);
}

void CardDrawTool::draw(const CardActor& actor)
{
    _program.setMatrix(_projectionMatrix * actor.modelViewMatrix);
    _program.setHighlight(actor.highlight);
    _program.enableTexture(false);
    _buffer.drawMiddle();
    _program.enableTexture(true);

    if (actor.isTopVisible)
    {
        _program._functions.glBindTexture(GL_TEXTURE_2D, actor.topTexture);
        _buffer.drawTop();
    }
    else
    {
        _program._functions.glBindTexture(GL_TEXTURE_2D, actor.bottomTexture);
        _buffer.drawBottom();
    }
}
