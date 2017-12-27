#include "MainWindow.hpp"
#include "GameState.hpp"
#include <QApplication>

int main(int argc, char *argv[])
{
    qDebug() << "size of CardState --" << sizeof(CardState);
    qDebug() << "size of CardInstance --" << sizeof(CardInstance);
    QApplication a(argc, argv);
    MainWindow w;
    //MainWidget w;
    w.show();
    
    return a.exec();
}
