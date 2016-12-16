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
QMAKE_CXXFLAGS += -Wall -Wextra  -Weffc++ -Wunknown-pragmas

SOURCES += main.cpp\
  	  	  	  	mainwindow.cpp \
  	  	click_lock_settings.cpp \
  	  	hotkey.cpp \
    mouse_catcher_thread.cpp

HEADERS  	+= mainwindow.h  	\
  	  	click_lock_settings.h \
  	  	hotkey.h \
    widget_input_event_catcher.h \
    mouse_catcher_thread.h

FORMS  	  	+= mainwindow.ui \
  	  	click_lock_settings.ui

RESOURCES += \
  	  	basic_resources.qrc
RC_FILE  	  	 = meta_data.rc
DISTFILES = meta_data.rc
