#-------------------------------------------------
#
# Project created by QtCreator 2013-06-19T18:03:56
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DEJARIX
TEMPLATE = app
CONFIG += c++11

SOURCES += main.cpp\
        MainWindow.cpp \
    MainWidget.cpp \
    CardSpecifications.cpp \
    CardBuilder.cpp \
    CardBuffer.cpp \
    CardActor.cpp \
    Camera.cpp \
    TableBuffer.cpp \
    CardDrawTool.cpp \
    MenuRing.cpp \
    MenuRingItem.cpp \
    BasicBuilder.cpp \
    BasicProgram.cpp \
    BasicBufferObject.cpp

HEADERS  += MainWindow.hpp \
    MainWidget.hpp \
    CardSpecifications.hpp \
    CardBuilder.hpp \
    CardBuffer.hpp \
    CardActor.hpp \
    Rotation.hpp \
    Camera.hpp \
    TableBuffer.hpp \
    CardDrawTool.hpp \
    MenuRing.hpp \
    MenuRingItem.hpp \
    BasicBuilder.hpp \
    BasicProgram.hpp \
    BasicBufferObject.hpp
