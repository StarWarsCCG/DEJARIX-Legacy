#ifndef PILELOCATIONS_HPP
#define PILELOCATIONS_HPP

#include <QVector2D>

struct PileLocations
{
    QVector2D reserveDeck;
    QVector2D forcePile;
    QVector2D usedPile;
    QVector2D lostPile;
    QVector2D outOutPlay;
};

#endif

