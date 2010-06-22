#pragma once
#include <vector>
#include "CSingleton.h"
#include <list>
#include "Data.h"
#include "BaseObject.h"

class ObjectManager : public CSingleton<ObjectManager> // : public BaseData
{
    friend class CSingleton<ObjectManager>;

public:
    ObjectManager();
	~ObjectManager();
	virtual void updateOM(); // call for bwapi
	void setGameSpeed(int speed); // call for qt appli
    void addObject(BaseObject* p);

protected:
	Data<int> speed; // temp var to do bwapi call in the good thread
	std::vector<BaseObject*> vObject;
};
