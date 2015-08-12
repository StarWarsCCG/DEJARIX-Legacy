#ifndef CARDFLIPANIMATION_HPP
#define CARDFLIPANIMATION_HPP

struct CardFlipAnimation
{
    int cardId;
    float firstRotation;
    float lastRotation;
    float firstFlip;
    float lastFlip;
    float magnitude;
    int stepCount;
    int currentStep;
};

#endif

