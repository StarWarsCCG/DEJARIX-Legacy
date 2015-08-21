#-------------------------------------------------
#
# Project created by QtCreator 2013-06-19T18:03:56
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = DEJARIX
TEMPLATE = app
CONFIG += c++11

SOURCES += main.cpp\
        MainWindow.cpp \
    MainWidget.cpp \
    CardBuilder.cpp \
    CardBuffer.cpp \
    CardActor.cpp \
    Camera.cpp \
    TableBuffer.cpp \
    BasicBuilder.cpp \
    BasicProgram.cpp \
    BasicBufferObject.cpp \
    TextureManager.cpp

HEADERS  += MainWindow.hpp \
    MainWidget.hpp \
    CardSpecifications.hpp \
    CardBuilder.hpp \
    CardBuffer.hpp \
    CardActor.hpp \
    Rotation.hpp \
    Camera.hpp \
    TableBuffer.hpp \
    BasicBuilder.hpp \
    BasicProgram.hpp \
    BasicBufferObject.hpp \
    TextureManager.hpp \
    CardPositionAnimation.hpp \
    CardRotationAnimation.hpp \
    DeferredArray.hpp
