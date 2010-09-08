#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <BWAPI.h>
#include <QtGui/QApplication>
#include "MainWindow.h"
#include "MicroProject.h"

MicroAIModule* broodAI = NULL;
#ifdef BW_QT_DEBUG
QApplication* qapplication = NULL;
MainWindow* qmainwindow = NULL;
bool g_onStartDone = false;
#endif

#define BUF_SIZE 255

static HANDLE hThreadMonitor;

DWORD WINAPI LaunchMonitor( LPVOID lpParam );

namespace BWAPI { Game* Broodwar; }
BOOL APIENTRY DllMain(HANDLE hModule, 
                      DWORD  ul_reason_for_call, 
                      LPVOID lpReserved)
{
	DWORD   dwThreadIdArray;

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        BWAPI::BWAPI_init();

        // Create the thread to begin execution on its own.
        hThreadMonitor = CreateThread( 
            NULL,                   // default security attributes
            0,                      // use default stack size  
            LaunchMonitor,       // thread function name
            NULL,          // argument to thread function 
            0,                      // use default creation flags 
            &dwThreadIdArray);   // returns the thread identifier 
        // Check the return value for success.
        // If CreateThread fails, terminate execution.
        // This will automatically clean up threads and memory. 
        if (hThreadMonitor == NULL) 
        {
            ExitProcess(3);
        }

        break;
    case DLL_PROCESS_DETACH:
#ifdef BW_QT_DEBUG
        qapplication->quit();        		
        // Wait until monitor thread have terminated.
        //WaitForSingleObject(hThreadMonitor, INFINITE);
        // Close all thread handles and free memory allocations.
        CloseHandle(hThreadMonitor);
#endif
        break;
    }
    return TRUE;
}

extern "C" __declspec(dllexport) BWAPI::AIModule* newAIModule(BWAPI::Game* game)
{
    BWAPI::Broodwar=game;
#ifdef BW_QT_DEBUG
    broodAI = new MicroAIModule(&qapplication);
#else
    broodAI = new MicroAIModule();
#endif
    return (MicroAIModule*)broodAI;
}

DWORD WINAPI LaunchMonitor(LPVOID lpParam)
{
#ifdef BW_QT_DEBUG
    while(!broodAI)
    {
        Sleep(50);
    }

    int argc = 1;
    char* name = "AI-Monitor";
    char** argv = &name;
    qapplication = new QApplication(argc, argv);
    qmainwindow = new MainWindow;

		while(!g_onStartDone)
		{
			Sleep(50);
		}

		qmainwindow->show();
		qmainwindow->initComponentsTree();
    qapplication->exec();
#endif

    return 0; 
}
