#-------------------------------------------------
#
# Project created by QtCreator 2016-09-30T16:13:44
#
#-------------------------------------------------

QT       += core gui network
CONFIG += C++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = 05-Tcp
TEMPLATE = app


SOURCES += main.cpp\
        server.cpp \
    client.cpp

HEADERS  += server.h \
    client.h

FORMS    += server.ui \
    client.ui
