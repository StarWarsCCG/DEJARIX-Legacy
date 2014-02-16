#ifndef CARD_HPP
#define CARD_HPP

#include <QVector>
#include <QDebug>
#include <random>

namespace StarWarsCCG
{
    class Card;

    class CardSequence
    {
    public:
        CardSequence();
        ~CardSequence();

        void addToBack(Card& card);
        void addToFront(Card& card);
        void remove(Card& card);
        int count() const;

        void testDump() const;

        template<class T> void shuffle(T& generator);

    private:
        Card* _firstCard;
    };

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
    void CardSequence::shuffle(T& generator)
    {
        int cardCount = count();
        qDebug() << "Card Count" << cardCount;

        if (cardCount > 1)
        {
            QVector<Card*> cards;
            cards.reserve(cardCount);

            while (_firstCard)
            {
                cards.append(_firstCard);
                remove(*_firstCard);
            }

            int last = cardCount - 1;

            for (int i = 0; i < last; ++i)
            {
                std::uniform_int_distribution<int> distribution(i + 1, last);
                int target = distribution(generator);
                qSwap(cards[i], cards[target]);
            }

            for (auto card : cards)
                addToFront(*card);
        }
    }
}

#endif
