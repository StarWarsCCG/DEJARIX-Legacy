#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <QtGlobal>
#include <QDateTime>
#include <map>
#include <vector>
#include <random>
#include "Piles.hpp"

constexpr quint8 LightSideFlag = 0x80;
constexpr quint8 LightSide(quint8 n) { return n | LightSideFlag; }
constexpr quint8 DarkSide(quint8 n) { return n & ~LightSideFlag; }
constexpr quint8 AutoSide(quint8 n, bool isLight)
{
    return isLight ? LightSide(n) : DarkSide(n);
}

// Absolute Collections
constexpr quint8 Locations = 0x00;
constexpr quint8 Starting = 0x01;
constexpr quint8 ReserveDeck = 0x02;
constexpr quint8 ForcePile = 0x03;
constexpr quint8 UsedPile = 0x04;
constexpr quint8 LostPile = 0x05;
constexpr quint8 OutOfPlay = 0x06;
constexpr quint8 Hand = 0x07;
constexpr quint8 Table = 0x08;
constexpr quint8 Reveal = 0x09;

// Relative Collections
constexpr quint8 LocationOccupation = 0x10;

struct CardState
{
    quint8 ordinal;
    quint8 indent;
    quint8 collection;
    quint8 rotation:2;
    bool ownedByLightSide:1;
    bool isFaceUp:1;
};

struct CardInstance
{
    quint8 id;
    CardState state;
};

struct CardDelta
{
    int milliseconds;
    int stateChangeCount;
};

struct Deck
{
    const qint64* cardIds;
    int cardCount;
    int startCount;
};

struct GameState
{
    int darkSideCount;
    int lightSideCount;
    qint64 cardInfoIdByCardInstanceId[256];
    CardState cardStateByInstanceId[256];

    std::vector<CardDelta> deltas;
    std::vector<CardInstance> stateChanges;
    std::mt19937 generator;
    QTime stopwatch;

    void start(Deck darkDeck, Deck lightDeck);

    void apply(CardInstance instance);
    void applyAll(const CardInstance* instances, int instanceCount);
    int countCollection(quint8 collectionId) const;
    quint8 topCard(quint8 collectionId) const;
};

#endif
