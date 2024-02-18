#-------------------------------------------------
#
# Project created by QtCreator 2016-05-10T10:09:33
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MyButton
TEMPLATE = app


SOURCES += main.cpp\
        mywidget.cpp \
    mybutton.cpp \
    mybuttonplus.cpp

HEADERS  += mywidget.h \
    mybutton.h \
    mybuttonplus.h

FORMS    += mywidget.ui

RESOURCES += \
    src.qrc
