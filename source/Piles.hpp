#ifndef PILES_HPP
#define PILES_HPP

template<class T> struct PileSet
{
    T reserveDeck;
    T forcePile;
    T usedPile;
    T lostPile;
    T outOfPlay;
};

template<class T> struct ExtendedPileSet
{
    T reveal;
    T table;
    T hand;
};

#endif

