#include "MainWindow.hpp"
#include <QKeyEvent>
#include <QDir>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
#ifdef Q_OS_MAC
    qDebug() << QDir::currentPath();
    QDir::setCurrent("../../..");
    qDebug() << QDir::currentPath();
#endif

    setCentralWidget(&_mainWidget);
    setWindowTitle("DEJARIX");
    resize(1024, 768);
}

MainWindow::~MainWindow()
{
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
    case Qt::Key_F11:
        toggleFullscreen();
        break;

    case Qt::Key_Escape:
        close();
        break;

    case Qt::Key_Space:
        _mainWidget.dump();
        break;

    default:
        _mainWidget.keyPressEvent(event);
        break;
    }
}

void MainWindow::contextMenuEvent(QContextMenuEvent* event)
{
    (void)event;
    //qDebug() << "context";
}

void MainWindow::toggleFullscreen()
{
    if (isFullScreen())
        showNormal();
    else
        showFullScreen();
}
