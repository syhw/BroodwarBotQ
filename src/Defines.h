#pragma once

#define BW_QT_DEBUG
#define __DEBUG_NICOLAS__

#ifdef BW_QT_DEBUG
#include <QtGui/QApplication>
#include "MainWindow.h"

extern QApplication* qapplication;
extern MainWindow* qmainwindow;
#endif