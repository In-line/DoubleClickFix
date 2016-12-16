#-------------------------------------------------
#
# Project created by QtCreator 2016-03-23T12:23:41
#
#-------------------------------------------------

QT  	  	  	 += core gui
CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DoubleClickFix
TEMPLATE = app
QMAKE_CXXFLAGS_RELEASE += -O2 -Werror
QMAKE_CXXFLAGS += -Wall -Wextra  -Wzero-as-null-pointer-constant -Weffc++ -Wunknown-pragmas

SOURCES += main.cpp\
  	  	  	  	mainwindow.cpp \
  	  	mousecatcherthread.cpp \
  	  	click_lock_settings.cpp \
  	  	hotkey.cpp

HEADERS  	+= mainwindow.h  	\
  	  	mousecatcherthread.h \
  	  	click_lock_settings.h \
  	  	hotkey.h

FORMS  	  	+= mainwindow.ui \
  	  	click_lock_settings.ui

RESOURCES += \
  	  	basic_resources.qrc
RC_FILE  	  	 = meta_data.rc
DISTFILES = meta_data.rc
