#include "ObjectManager.h"
#include <BWAPI.h>

ObjectManager::ObjectManager()
{

}

ObjectManager::~ObjectManager()
{

}

void ObjectManager::setGameSpeed( int speed)
{
	BWAPI::Broodwar->setLocalSpeed( speed);
}
