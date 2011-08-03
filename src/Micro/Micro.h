#pragma once
#include <Utils/CSingleton.h>
#include "Macro/BWSAL.h"
#include <BWTA.h>
#include <set>
#include "Micro/Goals/GoalManager.h"

class Micro : public CSingleton<Micro>
{
	friend class CSingleton<Micro>;
	Micro();
	~Micro();
	bool _launchedFirstPush;
public:
	GoalManager* goalManager;
	std::set<BWTA::Chokepoint*> ourChokes;
	BWTA::Chokepoint* frontChoke;
	void update();
	void onUnitCreate(BWAPI::Unit* unit);
	void onUnitShow(BWAPI::Unit* unit);
	void onUnitDestroy(BWAPI::Unit* unit);
	void onUnitRenegade(BWAPI::Unit* unit);
	void onNukeDetect(BWAPI::Position target);
};