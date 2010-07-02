#include <TimeManager.h>

TimeManager::TimeManager()
{

}

TimeManager::~TimeManager()
{

}

VOID CALLBACK test_callback( 
    HWND hwnd,        // handle to window for timer messages 
    UINT message,     // WM_TIMER message 
    UINT idTimer,     // timer identifier 
    DWORD dwTime)     // current system time 
{
    Broodwar->printf("timer: %d, %d", message, idTimer);
}


void TimeManager::update()
{
    frameCount++;
    // UINT_PTR WINAPI SetTimer(
    // __in_opt  HWND hWnd,
    // __in      UINT_PTR nIDEvent,
    // __in      UINT uElapse,
    // __in_opt  TIMERPROC lpTimerFunc
    // );
    SetTimer(
        NULL,   // handle to the window
        6669,   // reset the timer with the same ID
        40,     // 40 ms = 25 frames / sec
        test_callback //suspendAll // callback function when timer expires
        );
}

std::string TimeManager::getName() const
{
	return "TimeManager component";
}

void TimeManager::display() const
{
	Broodwar->drawTextScreen( 300, 5, "%d:%d", int(frameCount / 60) % 60, frameCount % 60);
}

unsigned long TimeManager::getElapsedTime() const
{
	return frameCount;
}

void TimeManager::suspendAll()
{
}
