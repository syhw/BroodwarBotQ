#pragma once
#include <vector>
#include "Data.inl"

class BaseObject;

class ObjectManager
{
public:
	ObjectManager();
	~ObjectManager();

	virtual void updateOM(); // call for bwapi
	void setGameSpeed(int speed); // call for qt appli

protected:
	Data<bool> speedChanged;
	Data<int> speed; // temp var to do bwapi call in the good thread
	std::vector<BaseObject*> vObject;
};
