#pragma once

//our defines
//#define __DEBUG__LOUIS
//#define __DEBUG_NICOLAS__
//#define __NON_IMPLEMENTE__
#define __DEBUG_GABRIEL__

#define BW_QT_DEBUG
#ifdef BW_QT_DEBUG
class QApplication;
class MainWindow;

extern QApplication* qapplication;
extern MainWindow* qmainwindow;
<<<<<<< HEAD
#endif 
=======
extern bool g_onStartDone;
#endif
>>>>>>> fb7e1948762ae3334742a0bf1157a942e8515d6e
