#include "MainWindow.hpp"
#include <QKeyEvent>
#include <QDir>
#include <QPushButton>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , _dock(tr("Chat"))
{
#ifdef Q_OS_MAC
    qDebug() << QDir::currentPath();
    QDir::setCurrent("../../..");
    qDebug() << QDir::currentPath();
#endif

    setCentralWidget(&_mainWidget);
    _dock.setWidget(&_chat);
    addDockWidget(Qt::RightDockWidgetArea, &_dock);
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
    case Qt::Key_Backslash:
        if (isFullScreen())
            showNormal();
        else
            showFullScreen();

        break;

    case Qt::Key_Escape:
        if (isFullScreen())
            showNormal();
        else
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
