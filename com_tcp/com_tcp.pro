#-------------------------------------------------
#
# Project created by QtCreator 2015-02-16T15:32:50
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = com_tcp
TEMPLATE = app


RC_FILE = appIco.rc

SOURCES += main.cpp\
        mainwindow.cpp \
    SerialPort/ManageSerialPort.cpp \
    SerialPort/qextserialbase.cpp \
    SerialPort/qextserialport.cpp \
    SerialPort/win_qextserialport.cpp \
    crc32/crc32.c \
    md5/md5.c \
    UserFile/uartWifi.c \
    thread/qSendSerial.cpp \
    frameData/frameData.c

HEADERS  += mainwindow.h \
    SerialPort/ManageSerialPort.h \
    SerialPort/qextserialbase.h \
    SerialPort/qextserialport.h \
    SerialPort/win_qextserialport.h \
    crc32/crc32.h \
    md5/md5.h \
    UserFile/uartWifi.h \
    link/list.h \
    thread/qSendSerial.h \
    frameData/frameData.h

FORMS    += mainwindow.ui

RESOURCES += \
    images.qrc
