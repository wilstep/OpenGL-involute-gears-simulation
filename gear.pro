#-------------------------------------------------
#
# Project created by QtCreator 2019-02-14T09:01:11
#
#-------------------------------------------------

# for linux
unix: QMAKE_CXXFLAGS += -std=c++17
# for windows
win32: CONFIG += c++17

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = gear
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    oglwidget.cpp \
    gear.cpp

HEADERS  += mainwindow.h \
    oglwidget.h \
    gear.h \
    myshaders.h

FORMS    += mainwindow.ui

DISTFILES += \
    gears.png

RESOURCES += \
    resource.qrc
