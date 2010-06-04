#include <TimeManager.h>

TimeManager::TimeManager()
{

}

TimeManager::~TimeManager()
{

}

void TimeManager::update()
{
	frameCount++;
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
