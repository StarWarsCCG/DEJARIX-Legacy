#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QDockWidget>
#include "MainWidget.hpp"
#include "ChatWidget.hpp"

class MainWindow : public QMainWindow
{
    Q_OBJECT

    MainWidget _mainWidget;
    QDockWidget _dock;
    ChatWidget _chat;

public:
    MainWindow(QWidget* parent = 0);
    virtual ~MainWindow();

protected:
    virtual void keyPressEvent(QKeyEvent* event);
    virtual void contextMenuEvent(QContextMenuEvent* event);
};

#endif
