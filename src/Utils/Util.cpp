#include "Util.h"
#include <fstream>
#include <stdarg.h>
#include <sys/stat.h>
#include <windows.h>
char buffer[1024];
void log(const char* text, ...)
{
  const int BUFFER_SIZE = 1024;
  char buffer[BUFFER_SIZE];

  va_list ap;
  va_start(ap, text);
  vsnprintf_s(buffer, BUFFER_SIZE, BUFFER_SIZE, text, ap);
  va_end(ap);

  FILE *outfile;
  if (fopen_s(&outfile, "bwapi-data\\logs\\BWSAL.log", "a+")==0)
  {
    fprintf_s(outfile, buffer);
    fclose(outfile);
  }
}

void myRestartGame()
{
	/////////// Launch an AutoHotkey that will click on "OK" in 10 sec /////////////
	char procName[200];
	sprintf_s(procName, "C:\\StarCraft\\AI\\BroodwarBotQ\\scripts\\startGame.exe");
	// Initialize StartupInfo structure
	STARTUPINFO    StartupInfo;
	memset(&StartupInfo, 0, sizeof(StartupInfo));
	StartupInfo.cb = sizeof(StartupInfo);

	// This will contain the information about the newly created process
	PROCESS_INFORMATION ProcessInformation;

	BOOL results = CreateProcess(0,
	                         procName, // Process and arguments
	                         0, // Process Attributes
	                         0, // Thread Attributes
	                         FALSE, // Inherit Handles
	                         0, // CreationFlags,
	                         0, // Environment
	                         0, // Current Directory
	                         &StartupInfo, // StartupInfo
	                         &ProcessInformation // Process Information
	                         );
	// Cleanup
	CloseHandle(ProcessInformation.hProcess);
	CloseHandle(ProcessInformation.hThread);
}

void logScore(bool isWinner, std::string mapName)
{
	char buffer[5];
	if (isWinner)
	{
		sprintf_s(buffer, "won");
	} 
	else
	{
		sprintf_s(buffer, "lost");
	}
	mapName = mapName.substr(mapName.find_last_of("\\") + 1, mapName.length() - mapName.find_last_of("\\") - 1);
	FILE *outfile;
	if (fopen_s(&outfile, "bwapi-data\\logs\\wins_loses.log", "a+")==0)
	{
		fprintf_s(outfile, "NMAI %s on map %s\n", buffer, mapName.c_str());
		fclose(outfile);
	}
}