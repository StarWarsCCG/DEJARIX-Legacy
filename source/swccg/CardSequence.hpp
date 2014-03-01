#ifndef CARDSEQUENCE_HPP
#define CARDSEQUENCE_HPP

namespace StarWarsCCG
{
    class Card;

    class CardSequence
    {
    public:
        CardSequence();
        CardSequence(CardSequence&& other);
        ~CardSequence();

        CardSequence& operator=(CardSequence&& other);

        void addToBack(Card& card);
        void addToFront(Card& card);
        Card* removeFront();
        void removeAll();
        int count() const;

        void testDump() const;

    private:
        CardSequence(const CardSequence&) = delete;
        CardSequence& operator=(const CardSequence&) = delete;

        Card* _firstCard;
        Card* _parentCard;

        friend class Card;
    };
}

#endif
