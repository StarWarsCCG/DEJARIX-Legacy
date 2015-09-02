#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QDockWidget>
#include <QTabWidget>
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

public:
    MainWindow(QWidget* parent = 0);
    virtual ~MainWindow();

protected:
    virtual void keyPressEvent(QKeyEvent* event);
    virtual void contextMenuEvent(QContextMenuEvent* event);
};

#endif
