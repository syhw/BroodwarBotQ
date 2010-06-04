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
  BWAPI::Broodwar->setLocalSpeed( speed.getValue());
	(*speedChanged.beginEdit()) = false;
	speedChanged.endEdit();
  BWAPI::Broodwar->printf( "changement de la vitesse du jeu pour %i", speed.getValue());
#endif
}

void ObjectManager::setGameSpeed( int speed)
{
	(*this->speed.beginEdit()) = speed;
	this->speed.endEdit();
	(*speedChanged.beginEdit()) = true;
	speedChanged.endEdit();
}
