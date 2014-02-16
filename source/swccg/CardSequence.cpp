#include "Card.hpp"
#include <QDebug>

namespace StarWarsCCG
{
    CardSequence::CardSequence()
        : _firstCard(nullptr)
    {
    }

    CardSequence::~CardSequence()
    {
    }

    void CardSequence::addToBack(Card& card)
    {
        card.remove();
        card._parentSequence = this;

        if (_firstCard)
        {
            Card* lastCard = _firstCard;

            while (lastCard->_nextSibling) lastCard = lastCard->_nextSibling;

            lastCard->_nextSibling = &card;
            card._previousSibling = lastCard;
        }
        else
        {
            _firstCard = &card;
        }
    }

    void CardSequence::addToFront(Card& card)
    {
        card.remove();
        card._parentSequence = this;

        card._nextSibling = _firstCard;
        _firstCard = &card;

        if (card._nextSibling)
            card._nextSibling->_previousSibling = &card;
    }

    void CardSequence::remove(Card& card)
    {
        if (card._parentSequence == this)
        {
            if (card._previousSibling)
                card._previousSibling->_nextSibling = card._nextSibling;
            else
                _firstCard = card._nextSibling;

            if (card._nextSibling)
                card._nextSibling->_previousSibling = card._previousSibling;

            card._parentSequence = nullptr;
            card._nextSibling = nullptr;
            card._previousSibling = nullptr;
        }
    }

    int CardSequence::count() const
    {
        int result = 0;

        for (Card* i = _firstCard; i; i = i->_nextSibling)
            ++result;

        return result;
    }

    void CardSequence::testDump() const
    {
        for (Card* i = _firstCard; i; i = i->_nextSibling)
            qDebug() << "card ID : " << i->testId();
    }
}
