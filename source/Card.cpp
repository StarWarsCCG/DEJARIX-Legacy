#include "Card.hpp"

Card::Card()
{
}

Card::Card(Card&& other)
    : _attachedCards(std::move(other._attachedCards))
{
}
