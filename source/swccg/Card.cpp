#include "Card.hpp"

namespace StarWarsCCG
{
    static int nextId = 1;

    Card::Card()
        : _parentSequence(nullptr)
        , _previousSibling(nullptr)
        , _nextSibling(nullptr)
    {
        _testId = nextId++;
    }

    Card::Card(Card&& other)
    {
        (void)other;
    }

    Card::~Card()
    {
    }

    Card& Card::operator=(Card&& other)
    {
        (void)other;
        return *this;
    }

    void Card::remove()
    {
        if (_parentSequence) _parentSequence->remove(*this);
    }
}
