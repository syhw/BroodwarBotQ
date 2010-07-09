#pragma once

//our defines
//#define __DEBUG__LOUIS
//#define __DEBUG_NICOLAS__
//#define __NON_IMPLEMENTE__
#define __DEBUG_GABRIEL__

#define BW_QT_DEBUG
#ifdef BW_QT_DEBUG
// We can add here common Qt includes but NONE of our headers !!!
#include <QtGui/QLabel>
class QApplication;
class MainWindow;

extern QApplication* qapplication;
extern MainWindow* qmainwindow;
extern bool g_onStartDone;
#endif
