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

INCLUDEPATH = ../libstkcomms/include ../libbarobo/include

SOURCES += src/main.cpp\
        src/dialog.cpp\
        src/listener.cpp

HEADERS  += src/dialog.h src/listener.h

FORMS    += ui/dialog.ui

*-g++* {
  QMAKE_CXXFLAGS=-std=c++11
}

win32:LIBS += -L../libstkcomms/build -lstkcomms -L../stage/lib -lbarobo -lws2_32
macx:LIBS += -L../libstkcomms/build -lstkcomms -L../stage/lib -lbarobo -framework CoreFoundation
