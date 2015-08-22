#-------------------------------------------------
#
# Project created by QtCreator 2013-06-19T18:03:56
#
#-------------------------------------------------

QT       += core gui widgets

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
    CardActor.cpp \
    Camera.cpp

HEADERS  += MainWindow.hpp \
    MainWidget.hpp \
    CardSpecifications.hpp \
    CardActor.hpp \
    Rotation.hpp \
    Camera.hpp \
    CardPositionAnimation.hpp \
    CardRotationAnimation.hpp \
    DeferredArray.hpp
