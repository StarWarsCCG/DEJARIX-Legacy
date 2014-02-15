#include "CardInSequence.hpp"

namespace StarWarsCCG
{
    CardInSequence::CardInSequence()
        : _isReversed(false)
    {
    }

    CardInSequence::CardInSequence(Card card, bool reversed)
        : _card(card)
        , _isReversed(reversed)
    {
    }

    CardInSequence::CardInSequence(const CardInSequence& other)
        : _card(other._card)
        , _isReversed(other._isReversed)
    {
    }

    CardInSequence::~CardInSequence()
    {
    }

    CardInSequence& CardInSequence::operator=(const CardInSequence& other)
    {
        _card = other._card;
        _isReversed = other._isReversed;
        return *this;
    }
}
