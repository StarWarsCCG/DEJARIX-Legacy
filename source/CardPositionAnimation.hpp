#ifndef CARDPOSITIONANIMATION_HPP
#define CARDPOSITIONANIMATION_HPP

#include <QVector3D>
#include "Curves.hpp"

struct CardPositionAnimation
{
    int cardId;
    ControlPoints<QVector3D, 3> control;
    int stepCount;
    int currentStep;
};

#endif
