# -------------------------------------------------
# Project created by QtCreator 2011-05-31T11:30:38
# -------------------------------------------------
QT +=           opengl
QT+=            core gui

greaterThan(QT_MAJOR_VERSION,4): QT += widgets
TARGET =        IhmCan

TEMPLATE =      app

SOURCES +=      src/main.cpp
SOURCES +=      src/mainwindowd.cpp
SOURCES +=      src/objectgl.cpp
SOURCES +=      src/mainwindow.cpp
SOURCES +=     
SOURCES +=     

HEADERS +=      include/mainwindowd.h
HEADERS +=      include/objectgl.h
HEADERS +=      include/mainwindow.h
HEADERS +=     
HEADERS +=     
FORMS +=        src/mainwindow.ui
INCLUDEPATH +=  src
INCLUDEPATH +=  include
LIBS        += -lpcan

OBJECTS_DIR = tmp/
MOC_DIR = tmp/
DESTDIR = bin/
