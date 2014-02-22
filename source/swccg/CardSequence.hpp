#ifndef CARDSEQUENCE_HPP
#define CARDSEQUENCE_HPP

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
        Card* removeFront();
        int count() const;

        void testDump() const;

    private:
        Card* _firstCard;
    };
}

#endif
