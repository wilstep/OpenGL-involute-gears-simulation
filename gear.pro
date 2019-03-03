#-------------------------------------------------
#
# Project created by QtCreator 2019-03-03T00:58:43
#
#-------------------------------------------------

# for linux
unix: QMAKE_CXXFLAGS += -std=c++14
# for windows
win32: CONFIG += c++14

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = gear
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp\
        gear.cpp\
        oglwidget.cpp \
        scroller.cpp

HEADERS  += myshaders.h \
        widget.h\
        gear.h\
        oglwidget.h\
        scroller.h

FORMS    += widget.ui

DISTFILES += \
    gears.png

RESOURCES += \
    resource.qrc
