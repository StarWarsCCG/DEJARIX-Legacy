#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <QtGlobal>
#include <QDateTime>
#include <vector>
#include <random>

constexpr quint8 LightSideFlag = 0x80;
constexpr quint8 LightSide(quint8 n) { return n | LightSideFlag; }
constexpr quint8 DarkSide(quint8 n) { return n & ~LightSideFlag; }

// Absolute Collections
constexpr quint8 Starting = 0x01;
constexpr quint8 ReserveDeck = 0x02;
constexpr quint8 ForcePile = 0x03;
constexpr quint8 UsedPile = 0x04;
constexpr quint8 LostPile = 0x05;
constexpr quint8 OutOfPlay = 0x06;
constexpr quint8 Hand = 0x07;
constexpr quint8 Table = 0x08;
constexpr quint8 Reveal = 0x09;

constexpr quint8 NullMode = 0x00;
constexpr quint8 CollectionMode = 0x01;
constexpr quint8 LocationMode = 0x02;
constexpr quint8 DarkOccupationMode = 0x3;
constexpr quint8 LightOccupationMode = 0x04;
constexpr quint8 AttachmentMode = 0x05;

struct CardState
{
    quint8 ordinal;
    quint8 location;
    quint8 mode:3;
    quint8 rotation:2;
    bool isFaceUp:1;
};

struct CardInstance
{
    quint8 id;
    CardState state;
};

struct CardDelta
{
    int stepIndex;
    CardInstance instance;
};

struct VisibleGameState
{
    qint64 cardInfoIdByVisibleId[256];
    CardState cardStateByVisibleId[256];
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
    quint8 cardInstanceIdByDarkVisibleId[256];
    quint8 cardInstanceIdByLightVisibleId[256];

    std::vector<int> millisecondsByStep;
    std::vector<CardDelta> deltas;
    std::mt19937_64 generator;
    QTime stopwatch;

    void start(Deck darkDeck, Deck lightDeck);

    void apply(CardInstance instance);
    void applyAll(const CardInstance* instances, int instanceCount);
    int countCollection(quint8 collectionId);
};

#endif
