#-------------------------------------------------
#
# Project created by QtCreator 2017-07-11T13:12:30
#
#-------------------------------------------------

QT       += core gui network
CONFIG+=c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DNSTester
TEMPLATE = app


SOURCES += src/main.cpp\
        src/mainwindow.cpp

HEADERS  += src/mainwindow.h

FORMS    += \
    src/mainwindow.ui

RESOURCES += \
    src/resource.qrc
