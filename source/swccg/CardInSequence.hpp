#ifndef CARDINSEQUENCE_HPP
#define CARDINSEQUENCE_HPP

#include "Card.hpp"

namespace StarWarsCCG
{
    class CardInSequence
    {
        public:
            CardInSequence();
            CardInSequence(Card card, bool reversed = false);
            CardInSequence(const CardInSequence& other);
            ~CardInSequence();

            CardInSequence& operator=(const CardInSequence& other);

            bool isReversed() const { return _isReversed; }
            void isReversed(bool reversed) { _isReversed = reversed; }
            void flip() { _isReversed = !_isReversed; }

        private:
            Card _card;
            bool _isReversed;
    };
}

#endif
