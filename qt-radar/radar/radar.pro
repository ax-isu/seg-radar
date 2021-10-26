#-------------------------------------------------
#
# Project created by QtCreator 2021-10-24T19:59:21
#
#-------------------------------------------------

QT       += core

QT       -= gui

QT       += network

TARGET = radar
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    storage_server.cpp \
    detector.cpp \
    dispatcher_one.cpp \
    radar_application.cpp

HEADERS += \
    storage_server.hpp \
    storage.hpp \
    simple_storage.hpp \
    detector.hpp \
    dispatcher.hpp \
    dispatcher_one.hpp \
    radar_application.hpp

QMAKE_CXXFLAGS += -std=c++17
