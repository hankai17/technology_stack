#-------------------------------------------------
#
# Project created by QtCreator 2016-10-06T10:10:35
#
#-------------------------------------------------

QT       += core gui \
        network
CONFIG += C++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = 01-Udp
TEMPLATE = app


SOURCES += main.cpp\
        udp1.cpp

HEADERS  += udp1.h

FORMS    += udp1.ui
