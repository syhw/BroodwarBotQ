#include <TimeManager.h>

TimeManager::TimeManager()
: BaseObject("TimeManager")
{

}

TimeManager::~TimeManager()
{

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
    
    return; // TODO => Get timerID (with hwnd?) right and test the exception throwing
    SetTimer(
        NULL,   // handle to the window
        6669,   // reset the timer with the same ID
        40,     // 40 ms = 25 frames / sec
        (TIMERPROC) this->suspendAll // callback function when timer expires
        );
}

std::string TimeManager::getName() const
{
	return "TimeManager component";
}

void TimeManager::display() const
{
	Broodwar->drawTextScreen(300, 5, "%d:%d", int(frameCount / 60) % 60, frameCount % 60);
}

void TimeManager::insert(TimeManaged* toManage)
{
    _managed.push_back(toManage);
}

const std::list<TimeManaged*>& TimeManager::getManaged() const
{
    return _managed;
}

VOID CALLBACK TimeManager::suspendAll(
                             HWND hwnd,        // handle to window for timer messages 
                             UINT message,     // WM_TIMER message 
                             UINT idTimer,     // timer identifier 
                             DWORD dwTime      // current system time 
                             )
{
    //TimeManager* tm = & TimeManager::Instance();
    // TODO access tm members like registred classes with an update/onFrame() function and kill them!!
    //for (std::list<TimeManaged*>::const_iterator it = tm->getManaged().begin();
    //    it != tm->getManaged().end(); ++it)
    //{
    //}
    throw (new TooLongException);
    Broodwar->printf("timer: %d, %d", message, idTimer); 
}

#ifdef BW_QT_DEBUG
QWidget* TimeManager::createWidget(QWidget* parent) const
{
	return new QLabel(QString("createWidget and refreshWidget undefined for this component."), parent);
}

void TimeManager::refreshWidget(QWidget* widget) const
{
// TODO update your widget after having defined it in the previous method :)
}
#endif