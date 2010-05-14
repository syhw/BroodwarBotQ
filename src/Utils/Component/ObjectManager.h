#pragma once
#include <vector>

class BaseObject;

class ObjectManager
{
public:
	ObjectManager();
	~ObjectManager();

	void setGameSpeed( int speed);

protected:
	std::vector<BaseObject*> vObject;
};
