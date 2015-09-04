#include "MainWindow.hpp"
#include "GameState.hpp"
#include <QApplication>

int main(int argc, char *argv[])
{
    qDebug() << sizeof(CardState);
    QApplication a(argc, argv);
    MainWindow w;
    //MainWidget w;
    w.show();
    
    return a.exec();
}
