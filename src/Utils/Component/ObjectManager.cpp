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

void ObjectManager::onFrame()
{
	for(std::vector<BaseObject*>::const_iterator it = vObject.begin(); it != vObject.end(); ++it)
		(*it)->onFrame();
}

void ObjectManager::addObject(BaseObject* p)
{
    vObject.push_back(p);
}
const std::vector<BaseObject*>& ObjectManager::getObjects() const
{
    return vObject;
}
