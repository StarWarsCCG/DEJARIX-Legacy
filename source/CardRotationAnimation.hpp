#ifndef CARDROTATIONANIMATION_HPP
#define CARDROTATIONANIMATION_HPP

struct CardRotationAnimation
{
    int cardId;
    float firstRadians;
    float lastRadians;
    float magnitude;
    int stepCount;
    int currentStep;
};

#endif

