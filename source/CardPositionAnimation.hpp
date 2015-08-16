#ifndef CARDPOSITIONANIMATION_HPP
#define CARDPOSITIONANIMATION_HPP

#include <QVector3D>

struct CardPositionAnimation
{
    int cardId;
    QVector3D firstPosition;
    QVector3D lastPosition;
    int stepCount;
    int currentStep;
};

#endif
