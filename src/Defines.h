#pragma once

//our defines
#define __DEBUG__LOUIS
#define __DEBUG_GABRIEL__

#define BW_QT_DEBUG
#ifdef BW_QT_DEBUG
#include <QtGui/QApplication>
#include "MainWindow.h"

extern QApplication* qapplication;
extern MainWindow* qmainwindow;
#endif