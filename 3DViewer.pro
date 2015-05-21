#-------------------------------------------------
#
# Project created by QtCreator 2015-05-17T12:45:06
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = 3DViewer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    openglwindow.cpp \
    renderwindow.cpp \
    camera.cpp \
    tiny_obj_loader.cc \
    scenehandler.cpp \
    object.cpp

HEADERS  += mainwindow.h \
    renderwindow.h \
    openglwindow.h \
    camera.h \
    tiny_obj_loader.h \
    scenehandler.h \
    object.h

FORMS    += mainwindow.ui

DISTFILES += \
    README.md
