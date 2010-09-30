#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <BWAPI.h>
#ifdef BW_QT_DEBUG
#include <QtGui/QApplication>
#endif
#include "MacroProject.h"
#include "Mainwindow.h"
#include "Defines.h"

BattleBroodAI* broodAI = NULL;

#ifdef BW_QT_DEBUG
QApplication* qapplication = NULL;
MainWindow* qmainwindow = NULL;
bool g_onStartDone = false;
#endif

#define BUF_SIZE 255

#ifdef BW_QT_DEBUG
static HANDLE  hThreadArrayMonitor;
DWORD WINAPI LaunchMonitor( LPVOID lpParam );
#endif

namespace BWAPI { Game* Broodwar; }
BOOL APIENTRY DllMain(HANDLE hModule,
                      DWORD  ul_reason_for_call,
                      LPVOID lpReserved)
{
#ifdef BW_QT_DEBUG
    DWORD   dwThreadIdArray;
#endif

	switch (ul_reason_for_call)
	{
	    case DLL_PROCESS_ATTACH:
          BWAPI::BWAPI_init();
#ifdef BW_QT_DEBUG
	        hThreadArrayMonitor = CreateThread( 
	        NULL,                   // default security attributes
	        0,                      // use default stack size  
	        LaunchMonitor,       // thread function name
	        NULL,          // argument to thread function 
	        0,                      // use default creation flags 
	        &dwThreadIdArray);   // returns the thread identifier 


            // Check the return value for success.
	        // If CreateThread fails, terminate execution. 
	        // This will automatically clean up threads and memory. 
	        if (hThreadArrayMonitor == NULL) 
	        {
	            ExitProcess(3);
	        }
#endif

		    break;
	    
        case DLL_PROCESS_DETACH:
#ifdef BW_QT_DEBUG
            if (qapplication)
                qapplication->quit();
					
            // Wait until monitor thread have terminated.
	        WaitForMultipleObjects(1, &hThreadArrayMonitor, TRUE, INFINITE);
	        // Close all thread handles and free memory allocations.
	        CloseHandle(&hThreadArrayMonitor);
#endif					
	        break;
	}

	return TRUE;
}

extern "C" __declspec(dllexport) BWAPI::AIModule* newAIModule(BWAPI::Game* game)
{
	BWAPI::Broodwar = game;
  broodAI = new BattleBroodAI();

#ifdef BW_QT_DEBUG
	while(!qmainwindow)
	{
		Sleep(50);
	}
#endif

	return broodAI;
}

#ifdef BW_QT_DEBUG
DWORD WINAPI LaunchMonitor(LPVOID lpParam ) 
{ 
	while(!broodAI)
	{
		Sleep(50);
	}

	int argc = 1;
	char* name = "AI-Monitor";
	char** argv = &name;
	qapplication = new QApplication(argc, argv);
    qmainwindow = new MainWindow();

	while(!g_onStartDone)
	{
		Sleep(50);
	}

	qmainwindow->show();
	qmainwindow->initComponentsTree();
	qapplication->exec();
	return 0; 
}
#endif