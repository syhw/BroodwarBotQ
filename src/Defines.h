#pragma once

#define BW_QT_DEBUG

#ifdef BW_QT_DEBUG
#include <QtGui/QApplication>
#include "MainWindow.h"

extern QApplication* qapplication;
extern MainWindow* qmainwindow;
extern bool g_onStartDone;
#endif
