#include "ObjectManager.h"
#include <BWAPI.h>

ObjectManager::ObjectManager()
{

}

ObjectManager::~ObjectManager()
{

}

void ObjectManager::updateOM()
{
#ifndef CREATOR_COMPIL
    if (speed.is_sychronized()) return;
        BWAPI::Broodwar->setLocalSpeed(99-speed.getValue());
    speed.synchronized();
    // BWAPI::Broodwar->printf( "Changement de la vitesse du jeu pour %i", speed.getValue());
#endif
}

void ObjectManager::setGameSpeed(int speed)
{
	(*this->speed.beginEdit()) = speed;
	this->speed.endEdit();
}
