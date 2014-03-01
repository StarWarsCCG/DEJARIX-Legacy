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

    Card* CardSequence::removeFront()
    {
        Card* result = _firstCard;

        if (result)
        {
            _firstCard = _firstCard->_nextSibling;
            if (_firstCard) _firstCard->_previousSibling = nullptr;

            result->_parentSequence = nullptr;
            result->_nextSibling = nullptr;
        }

        return result;
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
