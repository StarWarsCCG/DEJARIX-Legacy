#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QDockWidget>
#include <QTabWidget>
#include <QDir>
#include <QTcpSocket>
#include "MainWidget.hpp"
#include "ChatWidget.hpp"

class MainWindow : public QMainWindow
{
    Q_OBJECT

    MainWidget _mainWidget;
    QDockWidget _dock;
    QTabWidget _chatTabs;
    ChatWidget _playerChat;
    ChatWidget _publicChat;
    int _unreadChats = 0;
    QDir _dataDir;
    QTcpSocket _socket;
    QByteArray _data;
    bool _isReading = false;

    virtual void keyPressEvent(QKeyEvent* event);
    virtual void contextMenuEvent(QContextMenuEvent* event);

private slots:
    void openDataFolder();
    void about();
    void aboutQt();

    void onSocketConnect();
    void onSocketData();

public:
    MainWindow(QWidget* parent = 0);
    virtual ~MainWindow();

};

#endif
