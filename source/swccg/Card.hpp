#ifndef CARD_HPP
#define CARD_HPP

namespace StarWarsCCG
{
    class Card
    {
        public:
            Card();
            Card(const Card& other);
            ~Card();

            Card& operator=(const Card& other);

        private:
    };
}

#endif
