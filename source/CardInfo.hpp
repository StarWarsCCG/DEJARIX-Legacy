#ifndef CARDINFO_HPP
#define CARDINFO_HPP

#include <QString>

class CardInfo
{
    public:
        CardInfo();
        ~CardInfo();
    private:
        qlonglong _id;
        QString _title;
};

#endif
