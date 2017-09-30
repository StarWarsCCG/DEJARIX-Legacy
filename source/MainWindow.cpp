#include "MainWindow.hpp"
#include <QKeyEvent>
#include <QPushButton>
#include <QAction>
#include <QMessageBox>
#include <QMenuBar>
#include <QDesktopServices>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , _dock(tr("Chat"))
{
#ifdef Q_OS_MAC
    qDebug() << QDir::currentPath();
    QDir::setCurrent("../../..");
    qDebug() << QDir::currentPath();
#endif

    qDebug() << QDir::homePath();
    //QDir dir(QDir::homePath() + "/.local/share/DEJARIX");
    auto dir = QDir::home();
#ifdef Q_OS_WIN
    QString innerPath = QStringLiteral("DEJARIX");
#endif

#ifdef Q_OS_MAC
    QString innerPath = QStringLiteral("Library/Application Support/DEJARIX");
#endif

#ifdef Q_OS_LINUX
    QString innerPath = QStringLiteral(".local/share/DEJARIX");
#endif

    dir.mkdir(innerPath);
    dir.cd(innerPath);
    qDebug() << "path " << dir.absolutePath();
    _dataDir = dir;

    auto openDataFolderAction = new QAction(tr("&Open Data Folder"), this);
    connect(
        openDataFolderAction,
        SIGNAL(triggered(bool)),
        this,
        SLOT(openDataFolder()));

    auto aboutQtAction = new QAction(tr("About &Qt"), this);
    connect(
        aboutQtAction,
        SIGNAL(triggered(bool)),
        this,
        SLOT(aboutQt()));

    auto aboutAction = new QAction(tr("&About DEJARIX"), this);
    connect(aboutAction, SIGNAL(triggered(bool)), this, SLOT(about()));

    auto helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(openDataFolderAction);
    helpMenu->addAction(aboutQtAction);
    helpMenu->addAction(aboutAction);

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

void MainWindow::openDataFolder()
{
    auto path = QDir::toNativeSeparators(_dataDir.absolutePath());
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

void MainWindow::about()
{
    QMessageBox::about(this, "About DEJARIX", "DEJARIX version ZERO");
}

void MainWindow::aboutQt()
{
    QMessageBox::aboutQt(this);
}
