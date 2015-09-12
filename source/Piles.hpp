#ifndef PILES_HPP
#define PILES_HPP

#include <QVector2D>

struct Pile
{
    QVector2D position;
    int cardCount = 0;
};

struct PileSet
{
    Pile reserveDeck;
    Pile forcePile;
    Pile usedPile;
    Pile lostPile;
    Pile outOutPlay;
};

#endif
