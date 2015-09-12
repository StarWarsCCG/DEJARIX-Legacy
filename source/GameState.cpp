#include "GameState.hpp"

void GameState::start(Deck darkDeck, Deck lightDeck)
{
    for (int i = 0; i < darkDeck.startCount; ++i)
    {
        CardState cs;
        cs.ordinal = i;
        cs.location = DarkSide(Table);
        cs.mode = CollectionMode;
        cs.rotation = 0;
        cs.isFaceUp = true;

        cardStateByInstanceId[i] = cs;
        cardInfoIdByCardInstanceId[i] = darkDeck.cardIds[i];
    }

    for (int i = darkDeck.startCount; i < darkDeck.cardCount; ++i)
    {
        CardState cs;
        cs.ordinal = i;
        cs.location = DarkSide(ReserveDeck);
        cs.mode = CollectionMode;
        cs.rotation = 0;
        cs.isFaceUp = false;

        cardStateByInstanceId[i] = cs;
        cardInfoIdByCardInstanceId[i] = darkDeck.cardIds[i];
    }

    for (int i = 0; i < lightDeck.startCount; ++i)
    {
        CardState cs;
        cs.ordinal = i;
        cs.location = LightSide(Table);
        cs.mode = CollectionMode;
        cs.rotation = 0;
        cs.isFaceUp = true;

        int ii = darkDeck.cardCount + i;
        cardStateByInstanceId[ii] = cs;
        cardInfoIdByCardInstanceId[ii] = lightDeck.cardIds[i];
    }

    for (int i = lightDeck.startCount; i < lightDeck.cardCount; ++i)
    {
        CardState cs;
        cs.ordinal = i;
        cs.location = LightSide(ReserveDeck);
        cs.mode = CollectionMode;
        cs.rotation = 0;
        cs.isFaceUp = false;

        int ii = darkDeck.cardCount + i;
        cardStateByInstanceId[ii] = cs;
        cardInfoIdByCardInstanceId[ii] = lightDeck.cardIds[i];
    }

    darkSideCount = darkDeck.cardCount;
    lightSideCount = lightDeck.cardCount;
    stopwatch.start();
    generator.seed(QDateTime::currentMSecsSinceEpoch());
}

void GameState::apply(CardInstance instance)
{
    applyAll(&instance, 1);
}

void GameState::applyAll(const CardInstance* instances, int instanceCount)
{
    auto stepIndex = (int)millisecondsByStep.size();

    for (int i = 0; i < instanceCount; ++i)
    {
        auto instance = instances[i];
        cardStateByInstanceId[instance.id] = instance.state;
        deltas.push_back({stepIndex, instance});
    }

    millisecondsByStep.push_back(stopwatch.elapsed());
}

int GameState::countCollection(quint8 collectionId) const
{
    int result = 0;
    int totalCount = darkSideCount + lightSideCount;

    for (int i = 0; i < totalCount; ++i)
    {
        const CardState& cs = cardStateByInstanceId[i];
        if (cs.mode == CollectionMode && cs.location == collectionId) ++result;
    }

    return result;
}

quint8 GameState::topCard(quint8 collectionId) const
{
    quint8 result = 255;
    int ordinal = -1;
    int totalCount = darkSideCount + lightSideCount;

    for (int i = 0; i < totalCount; ++i)
    {
        const CardState& cs = cardStateByInstanceId[i];
        if (cs.mode == CollectionMode
            && cs.location == collectionId
            && cs.ordinal > ordinal)
        {
            result = i;
            ordinal = cs.ordinal;
        }
    }

    return result;
}
