#-------------------------------------------------
#
# Project created by QtCreator 2016-09-27T14:50:12
#
#-------------------------------------------------

QT       += core gui
CONFIG += C++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = 04-QDialog
TEMPLATE = app


SOURCES += main.cpp\
        dialog.cpp \
    mydialog.cpp

HEADERS  += dialog.h \
    mydialog.h

FORMS    += dialog.ui \
    mydialog.ui
