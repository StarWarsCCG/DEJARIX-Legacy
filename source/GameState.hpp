#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <QtGlobal>
#include <QTime>
#include <vector>

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
constexpr quint8 LocationRow = 0x10;

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
    quint8 instanceId;
    CardState instanceData;
};

struct CardDelta
{
    int stepIndex;
    CardInstance instance;
};

class GameState
{
    int _darkSideCount = 0;
    int _lightSideCount = 0;
    qint64 _cardInfoIdByCardInstanceId[256];
    CardState _cardStateByInstanceId[256];

    std::vector<int> _millisecondsByStep;
    std::vector<CardDelta> _deltas;
    QTime _stopwatch;

public:
    GameState() = default;
    GameState(GameState&&) = default;
    GameState(const GameState&) = default;
    ~GameState() = default;

    GameState& operator=(GameState&&) = default;
    GameState& operator=(const GameState&) = default;

    void apply(CardInstance instance);
    void applyAll(const CardInstance* instances, int instanceCount);
};

#endif
