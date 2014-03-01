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
        : _parentSequence(other._parentSequence)
        , _previousSibling(other._previousSibling)
        , _nextSibling(other._nextSibling)
    {
        _testId = nextId++;

        other._parentSequence = nullptr;
        other._previousSibling = nullptr;
        other._nextSibling = nullptr;

        if (_previousSibling)
            _previousSibling->_nextSibling = this;
        else
            _parentSequence->_firstCard = this;

        if (_nextSibling) _nextSibling->_previousSibling = this;
    }

    Card::~Card()
    {
        remove();
    }

    Card& Card::operator=(Card&& other)
    {
        if (this != &other)
        {
            remove();

            _parentSequence = other._parentSequence;
            _previousSibling = other._previousSibling;
            _nextSibling = other._nextSibling;

            other._parentSequence = nullptr;
            other._previousSibling = nullptr;
            other._nextSibling = nullptr;

            if (_previousSibling)
                _previousSibling->_nextSibling = this;
            else
                _parentSequence->_firstCard = this;

            if (_nextSibling) _nextSibling->_previousSibling = this;
        }

        return *this;
    }

    void Card::remove()
    {
        if (_parentSequence)
        {
            if (_previousSibling)
                _previousSibling->_nextSibling = _nextSibling;
            else
                _parentSequence->_firstCard = _nextSibling;

            if (_nextSibling)
                _nextSibling->_previousSibling = _previousSibling;

            _parentSequence = nullptr;
            _nextSibling = nullptr;
            _previousSibling = nullptr;
        }
    }
}
