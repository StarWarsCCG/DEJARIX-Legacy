#ifndef CARD_HPP
#define CARD_HPP

#include <QVector>

class Card
{
    public:
        Card();
        Card(Card&& other);
        ~Card() = default;

    private:
        QVector<Card> _attachedCards;
};

#endif
