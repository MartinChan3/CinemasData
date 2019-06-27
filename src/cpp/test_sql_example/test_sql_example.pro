#-------------------------------------------------
#
# Project created by QtCreator 2019-06-25T09:16:29
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = test_sql_example
TEMPLATE = app

#配置输出目录
win32:CONFIG(release, debug|release): DESTDIR = ../../../OUTPUT/release
else:win32:CONFIG(debug, debug|release): DESTDIR = ../../../OUTPUT/debug
else:unix:DESTDIR = ../../OUTPUT

win32:CONFIG(release, debug|release): MOC_DIR = ./Debug
else:win32:CONFIG(debug, debug|release): MOC_DIR = ./Release

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h \
    connection.h

FORMS    += mainwindow.ui
