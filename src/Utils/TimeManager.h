#pragma once
#include <windows.h>
#include <CSingleton.h>
#include <BWAPI.h>
#include <string>
#include <exception>
#include "BaseObject.h"

using namespace BWAPI;

struct TimeManaged;

class TooLongException: public std::exception
{
    virtual const char* what() const throw()
    {
        return "an update function took more than 40ms";
    }
};

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
* All classes running an update/onFrame() in BWAPI's thread should register here
* To be terminated if they take more than 1/25 sec (40ms) to run (and so slow down the game).
*/
class TimeManager : public CSingleton<TimeManager>, public BaseObject
{
	friend class CSingleton<TimeManager>;

private:
	TimeManager();
	~TimeManager();
    std::list<TimeManaged*> _managed;

public:
	// Common interface
	void update();
	std::string getName() const;
	void display() const; // display ellapsed TimeManager.
    void insert(TimeManaged* toManage);
    const std::list<TimeManaged*>& getManaged() const;

	// TimeManager
    unsigned long getElapsedTime() const;
    static VOID CALLBACK suspendAll( // static for the purpose of TIMERPROC C function call
        HWND hwnd,        // handle to window for timer messages 
        UINT message,     // WM_TIMER message 
        UINT idTimer,     // timer identifier 
        DWORD dwTime      // current system time 
        );

#ifdef BW_QT_DEBUG
    // Qt interface
    virtual QWidget* createWidget(QWidget* parent) const;
    virtual void refreshWidget(QWidget* widget) const;
#endif

private:
    unsigned long frameCount; // frame count since the begining
};
