#ifndef CARD_HPP
#define CARD_HPP

#include "CardSequence.hpp"
#include <QVector>
#include <QDebug>
#include <random>

namespace StarWarsCCG
{
    class Card
    {
    public:
        Card();
        Card(Card&& other);
        ~Card();

        Card& operator=(Card&& other);

        void remove();

        int testId() const { return _testId; }

    private:
        Card(const Card&) = delete;
        Card& operator=(const Card&) = delete;

        int _testId;

        CardSequence _attachedCards;

        CardSequence* _parentSequence;
        Card* _previousSibling;
        Card* _nextSibling;

        friend class CardSequence;
    };

    template<class T>
    void shuffle(CardSequence& sequence, T& generator)
    {
        int cardCount = sequence.count();
        qDebug() << "Card Count" << cardCount;

        if (cardCount > 1)
        {
            QVector<Card*> cards;
            cards.reserve(cardCount);

            while (Card* card = sequence.removeFront()) cards.append(card);

            int last = cardCount - 1;

            for (int i = 0; i < last; ++i)
            {
                std::uniform_int_distribution<int> distribution(i, last);
                int target = distribution(generator);
                qSwap(cards[i], cards[target]);
            }

            for (auto card : cards)
                sequence.addToFront(*card);
        }
    }
}

#endif
