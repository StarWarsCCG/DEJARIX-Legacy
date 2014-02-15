#include "CardSequence.hpp"
#include <QtGlobal>
#include <random>

namespace StarWarsCCG
{
    CardSequence::CardSequence()
    {
    }

    CardSequence::~CardSequence()
    {
    }

    void CardSequence::addToBack(const CardInSequence& cardInSequence)
    {
        _cards.append(cardInSequence);
    }

    void CardSequence::addToFront(const CardInSequence& cardInSequence)
    {
        _cards.prepend(cardInSequence);
    }
}
