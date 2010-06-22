#include "ObjectManager.h"
#include "Data.inl"
#include <BWAPI.h>
#include "BaseObject.h"

ObjectManager::ObjectManager()
: speed(1, "speed", 0)
{
    if (Inst)
        std::cerr << "Tu fais de la merde mec, t'instancies un pauvre singleton une seconde fois juste parce qu'il a la faiblesse d'avoir un constructeur en public !!" << std::endl; 
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

void ObjectManager::addObject(BaseObject* p)
{
    vObject.push_back(p);
}