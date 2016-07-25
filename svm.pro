QT += core
QT -= gui

CONFIG += c++11

TARGET = svm
CONFIG += console PKGCONFIG
PKGCONFIG += opencv
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp
