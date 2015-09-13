#ifndef PILES_HPP
#define PILES_HPP

template<typename T> struct PileSet
{
    T reserveDeck;
    T forcePile;
    T usedPile;
    T lostPile;
    T outOfPlay;
};

#endif

