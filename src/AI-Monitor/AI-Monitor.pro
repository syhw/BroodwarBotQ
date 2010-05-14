# -------------------------------------------------
# Project created by QtCreator 2010-04-22T21:31:47
# -------------------------------------------------
QT += opengl \
    xml
CONFIG += qt \
    opengl \
    release
TARGET = AI-Monitor
TEMPLATE = app

INCLUDEPATH += ../../../BWAPI_Beta_2.6.1/include ../Utils/Component
LIBS += -L"../../../BWAPI_Beta_2.6.1/lib" \
    -lBWAPI

SOURCES += main.cpp \
    Mainwindow.cpp \
    MiniMap.cpp \
    menuwidget.cpp

HEADERS += Mainwindow.h \
    MiniMap.h \
    menuwidget.h

FORMS += Mainwindow.ui \
    menuwidget.ui
