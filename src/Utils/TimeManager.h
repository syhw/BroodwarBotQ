#pragma once
#include <windows.h>
#include <CSingleton.h>
#include <BWAPI.h>
#include <string>

using namespace BWAPI;


/** Special exception to abort a thread gently
*/
/*class ThreadAbort
{
	__declspec (noreturn) static void Throw();
public:
	static bool RaiseInThread(HANDLE hThread);
	static void DontOptimize() throw (...);
};*/


/**
* The goal of this class is to provide tools related to TimeManager.
*
* The frame count since the begining can be retrieved.
* Functions can be called back after a given frames.
* Functions can be called back with a given frequency.
*/

// Le but est de repartir la charge entre les differentes fonctions call back. Mais est ce utile si on threade ?

class TimeManager : public CSingleton<TimeManager> 
{
	friend class CSingleton<TimeManager>;

private:
	TimeManager();
	~TimeManager();

public:
	// Common interface
	void update();
	std::string getName() const;
	void display() const; // display ellapsed TimeManager.

	// TimeManager
	unsigned long getElapsedTime() const;
    void suspendAll();

	// TimeManagerr facilities
  //void call();

private:
	unsigned long frameCount; // frame count since the begining
};
