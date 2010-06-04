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

DEFINES += CREATOR_COMPIL

INCLUDEPATH += ../../../BWAPI_Beta_2.7.2/include \
	../Utils/Component
LIBS += -L"../../../BWAPI_Beta_2.7.2/lib" \
    -L"../Utils/Component" \
    -lBWAPI

SOURCES += main.cpp \
    Mainwindow.cpp \
    MiniMap.cpp \
    menuwidget.cpp \
    ../Utils/Component/BaseData.cpp \
    ../Utils/Component/BaseObject.cpp \
    ../Utils/Component/ObjectManager.cpp

HEADERS += Mainwindow.h \
    MiniMap.h \
    menuwidget.h \
    ../Utils/Component/BaseData.h \
    ../Utils/Component/BaseObject.h \
    ../Utils/Component/CustomOStream.h \
    ../Utils/Component/Data.inl \
    ../Utils/Component/Data.h \
    ../Utils/Component/ObjectManager.h

FORMS += Mainwindow.ui \
    menuwidget.ui
