#-------------------------------------------------
#
# Project created by QtCreator 2017-07-11T13:12:30
#
#-------------------------------------------------


QT  += core gui network widgets
QT  +=dbus

TARGET = dns-tester
TEMPLATE = app
VERSION=1.3

CONFIG+=c++11
CONFIG+=link_pkgconfig
PKGCONFIG+=dtkwidget


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

DISTFILES += \
    dns-tester.desktop

isEmpty(PREFIX){
    PREFIX=/usr
}

target.path=$$PREFIX/bin

icon.path=$$PREFIX/share/pixmaps
icon.files=$$PWD/src/resource/icon/dns-tester.png

desktop.path=$$PREFIX/share/applications
desktop.files= $$PWD/dns-tester.desktop


INSTALLS += target icon desktop
