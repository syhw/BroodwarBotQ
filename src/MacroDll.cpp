#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <BWAPI.h>
#include <QtGui/QApplication>
#include "MainWindow.h"
#include "MacroProject.h"

#define USE_MONITOR
#define BUF_SIZE 255

volatile static BattleBroodAI* broodAI = NULL;
static QApplication* application = NULL;
static HANDLE  hThreadArrayMonitor;


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

		    break;
	    
        case DLL_PROCESS_DETACH:
			// Wait until monitor thread have terminated.

	        application->quit();
        		
	        WaitForMultipleObjects(1, &hThreadArrayMonitor, TRUE, INFINITE);

	        // Close all thread handles and free memory allocations.
            
	        CloseHandle(&hThreadArrayMonitor);

	        break;
	}

	return TRUE;
}

extern "C" __declspec(dllexport) BWAPI::AIModule* newAIModule(BWAPI::Game* game)
{
	BWAPI::Broodwar=game;
	broodAI = new BattleBroodAI();
	return (BattleBroodAI*)broodAI;
}


DWORD WINAPI LaunchMonitor(LPVOID lpParam ) 
{ 
#ifdef USE_MONITOR

	while(!broodAI)
	{
		Sleep(50);
	}

	int argc = 1;
	char* name = "AI-Monitor";
	char** argv = &name;
	application = new QApplication(argc, argv);
    MainWindow w(0, (BattleBroodAI*)broodAI);
	w.show();
	application->exec();
#endif

	return 0; 
}
