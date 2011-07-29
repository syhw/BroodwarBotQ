#pragma once
#include <Utils/CSingleton.h>
#include "Macro/BWSAL.h"
#include "Micro/Goals/GoalManager.h"

class Micro : public CSingleton<Micro>
{
	friend class CSingleton<Micro>;
	Micro();
	~Micro();
public:
	GoalManager* goalManager;
	void update();
	void onUnitCreate(BWAPI::Unit* unit);
	void onUnitShow(BWAPI::Unit* unit);
	void onUnitDestroy(BWAPI::Unit* unit);
};