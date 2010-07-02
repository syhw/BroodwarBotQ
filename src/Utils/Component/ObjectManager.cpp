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
}

void ObjectManager::setGameSpeed(int speed)
{
#ifndef CREATOR_COMPIL
    BWAPI::Broodwar->setLocalSpeed(speed);
#endif
}

void ObjectManager::addObject(BaseObject* p)
{
    vObject.push_back(p);
}
const std::vector<BaseObject*>& ObjectManager::getObjects() const
{
    return vObject;
}
