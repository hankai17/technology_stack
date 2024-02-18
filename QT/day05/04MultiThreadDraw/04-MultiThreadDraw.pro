#-------------------------------------------------
#
# Project created by QtCreator 2016-10-06T14:53:41
#
#-------------------------------------------------

QT       += core gui
CONFIG += C++11
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = 04-MultiThreadDraw
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    drawimage.cpp

HEADERS  += widget.h \
    drawimage.h

FORMS    += widget.ui
