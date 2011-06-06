#pragma once
#include <Utils/CSingleton.h>
#include "BWSAL.h"
#include "GoalManager.h"
#include "WarManager.h"

class Micro : public CSingleton<Micro>
{
	friend class CSingleton<Micro>;
	Micro();
	~Micro();
public:
	GoalManager* goalManager;
	WarManager* warManager;
	void update();
	void onUnitCreate(BWAPI::Unit* unit);
	void onUnitDestroy(BWAPI::Unit* unit);
};