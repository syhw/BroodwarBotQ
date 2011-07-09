#pragma once
#include <vector>
#include "CSingleton.h"
#include <list>
#include "Data.h"
#include "BaseObject.h"

class ObjectManager : public CSingleton<ObjectManager>
{
    friend class CSingleton<ObjectManager>;

public:
    ObjectManager();
	~ObjectManager();
	virtual void onFrame(); // call for bwapi
	void setGameSpeed(int speed); // call for qt appli
    void addObject(BaseObject* p);
    const std::vector<BaseObject*>& getObjects() const;

protected:
  	Data<int> speed; // temp var to do bwapi call in the good thread
	std::vector<BaseObject*> vObject;
};