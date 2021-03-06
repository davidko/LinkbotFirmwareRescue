#-------------------------------------------------
#
# Project created by QtCreator 2014-01-27T10:59:16
#
#-------------------------------------------------

QT       += core gui

#CONFIG += console

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LinkbotFirmwareRescue
TEMPLATE = app

INCLUDEPATH = ../libstkcomms ../libbarobo/include

SOURCES += main.cpp\
        dialog.cpp\
        listener.cpp

HEADERS  += dialog.h listener.h

FORMS    += dialog.ui

LIBS += -L../libstkcomms/build -lstkcomms -L../stage/lib -lbarobo -lws2_32
