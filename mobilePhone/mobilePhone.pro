#-------------------------------------------------
#
# Project created by QtCreator 2015-02-24T13:40:47
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mobilePhone
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    client/client.cpp \
    crc32/crc32.c \
    md5/md5.c \
    frameData/frameData.c

HEADERS  += mainwindow.h \
    client/client.h \
    crc32/crc32.h \
    md5/md5.h \
    frameData/frameData.h

FORMS    += mainwindow.ui

CONFIG += mobility
MOBILITY = 

