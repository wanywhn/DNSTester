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


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += \
    mainwindow.ui

RESOURCES += \
    resource.qrc
