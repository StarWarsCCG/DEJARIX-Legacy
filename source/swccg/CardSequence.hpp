#ifndef CARDSEQUENCE_HPP
#define CARDSEQUENCE_HPP

#include "CardInSequence.hpp"
#include <QVector>

namespace StarWarsCCG
{
    class CardSequence
    {
        public:
            CardSequence();
            ~CardSequence();

            void addToBack(const CardInSequence& card);
            void addToFront(const CardInSequence& cardInSequence);

            template<class T> void shuffle(T& generator)
            {
                int last = _cards.size() - 1;

                for (int i = 0; i < last; ++i)
                {
                    std::uniform_int_distribution<int> distribution(i + 1, last);
                    int target = distribution(generator);
                    qSwap(_cards[i], _cards[target]);
                }
            }

        private:
            QVector<CardInSequence> _cards;
    };
}

#endif
