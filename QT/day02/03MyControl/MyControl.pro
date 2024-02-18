#-------------------------------------------------
#
# Project created by QtCreator 2016-09-27T11:38:45
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MyControl
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    smallwidget.cpp \
    mybutton.cpp \
    mybtn.cpp

HEADERS  += mainwindow.h \
    smallwidget.h \
    mybutton.h \
    mybtn.h

FORMS    += mainwindow.ui \
    smallwidget.ui \
    mybutton.ui
