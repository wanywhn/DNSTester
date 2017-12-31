#-------------------------------------------------
#
# Project created by QtCreator 2017-07-11T13:12:30
#
#-------------------------------------------------

QT       += core gui network
QT  +=dbus
CONFIG+=c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DNSTester
TEMPLATE = app

unix{
    CONFIG+=link_pkgconfig
    PKGCONFIG+=dtkwidget
}

SOURCES += src/main.cpp\
        src/mainwindow.cpp \
    src/networkutils.cpp \
    src/multlistitem.cpp \
    src/multilistview.cpp

HEADERS  += src/mainwindow.h \
    src/networkutils.h \
    src/multlistitem.h \
    src/multilistview.h

FORMS    += \
    src/mainwindow.ui

RESOURCES += \
    src/resource.qrc
#LIBS+=-ldtkwidget
