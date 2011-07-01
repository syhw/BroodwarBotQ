#pragma once
#include "BWAPI.h"
#include "Utils/CSingleton.h"
#include "serialized_tables.h"

class ETechEstimator : public CSingleton<ETechEstimator>
{
    friend class CSingleton<ETechEstimator>;
    ETechEstimator();
    ~ETechEstimator();
	serialized_tables st;
	std::set<BWAPI::Unit*> buildingsSeen; 
	std::set<int> buildingsTypesSeen;
	bool notFirstOverlord;
	bool insertBuilding(BWAPI::Unit* u);
	bool insertBuilding(BWAPI::UnitType ut);
    void computeDistribOpenings(int time);
public:
	void onUnitDestroy(BWAPI::Unit* u);
	void onUnitShow(BWAPI::Unit* u);
	void onUnitHide(BWAPI::Unit* u);
};