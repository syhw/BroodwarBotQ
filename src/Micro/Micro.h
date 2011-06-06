#pragma once
#include <Utils/CSingleton.h>
#include "Macro/BWSAL.h"
#include "Micro/Goals/GoalManager.h"
#include "Micro/WarManager.h"

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