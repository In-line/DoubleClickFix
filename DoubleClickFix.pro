#-------------------------------------------------
#
# Project created by QtCreator 2016-03-23T12:23:41
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DoubleClickFix
TEMPLATE = app
QMAKE_CXXFLAGS_RELEASE += /O2


SOURCES += main.cpp\
        mainwindow.cpp \
    mousecatcherthread.cpp

HEADERS  += mainwindow.h \
    mousecatcherthread.h

FORMS    += mainwindow.ui

RESOURCES += \
    basic_resources.qrc
RC_FILE     = meta_data.rc
