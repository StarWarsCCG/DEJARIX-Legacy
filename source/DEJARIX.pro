#-------------------------------------------------
#
# Project created by QtCreator 2013-06-19T18:03:56
#
#-------------------------------------------------

QT       += core gui widgets network

TARGET = DEJARIX
TEMPLATE = app
CONFIG += c++11

!macx:!win32 {
    QMAKE_CXXFLAGS += -fsanitize=address -fno-omit-frame-pointer
    QMAKE_LFLAGS += -fsanitize=address
}

SOURCES += main.cpp\
        MainWindow.cpp \
    MainWidget.cpp \
    Camera.cpp \
    ChatWidget.cpp \
    GameState.cpp

HEADERS  += MainWindow.hpp \
    MainWidget.hpp \
    CardSpecifications.hpp \
    CardActor.hpp \
    Rotation.hpp \
    Camera.hpp \
    CardPositionAnimation.hpp \
    CardRotationAnimation.hpp \
    DeferredArray.hpp \
    MatrixAnimation.hpp \
    ChatWidget.hpp \
    Curves.hpp \
    GameState.hpp \
    Piles.hpp
