#include "ObjectManager.h"
#include "Data.inl"
#include <BWAPI.h>
#include "BaseObject.h"

ObjectManager::ObjectManager()
: speed(1, "speed", 0)
{
}

ObjectManager::~ObjectManager()
{

}

void ObjectManager::updateOM()
{
#ifndef CREATOR_COMPIL
    if (speed.is_sychronized()) return;
    BWAPI::Broodwar->setLocalSpeed(99 - *speed);
    BWAPI::Broodwar->printf("99 - speed: %d", 99 - *speed);
    speed.synchronized();
    // BWAPI::Broodwar->printf( "Changement de la vitesse du jeu pour %i", speed.getValue());
#endif
}

void ObjectManager::setGameSpeed(int speed)
{
    //BWAPI::Broodwar->setLocalSpeed(99 - speed);
    //BWAPI::Broodwar->printf("99 - speed: %d", 99 - speed);
    //return;
	(*this->speed.beginEdit()) = speed;
	this->speed.endEdit();
}

void ObjectManager::addObject(BaseObject* p)
{
    vObject.push_back(p);
}
const std::vector<BaseObject*>& ObjectManager::getObjects() const
{
    return vObject;
}
