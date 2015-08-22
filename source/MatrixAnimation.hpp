#ifndef MATRIXANIMATION_HPP
#define MATRIXANIMATION_HPP

#include <QMatrix4x4>

struct MatrixAnimation
{
    QMatrix4x4 firstMatrix;
    QMatrix4x4 lastMatrix;
    int stepCount;
    int currentStep;
};

#endif

