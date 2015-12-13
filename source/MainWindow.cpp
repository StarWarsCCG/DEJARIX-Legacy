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

    setFocusPolicy(Qt::WheelFocus);

    connect(&_playerChat, SIGNAL(emptyChatSent()), this, SLOT(setFocus()));
    connect(&_publicChat, SIGNAL(emptyChatSent()), this, SLOT(setFocus()));
    connect(
        &_mainWidget,
        SIGNAL(cardEvent(QString)),
        &_playerChat,
        SLOT(onCardEvent(QString)));

    _chatTabs.addTab(&_playerChat, "Players");
    _chatTabs.addTab(&_publicChat, "Spectators");

    setCentralWidget(&_mainWidget);
    _dock.setWidget(&_chatTabs);
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
    case Qt::Key_Backslash:
    //case Qt::Key_Enter:
    //case Qt::Key_Return:
        qDebug() << "wat";
        if (_dock.isHidden()) _dock.show();
        _chatTabs.setCurrentIndex(0);
        _playerChat.focusChat();
        break;

    case Qt::Key_F11:
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

        _chatTabs.setTabText(1, "Spectators (13)");
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
