#pragma once
#include "BWAPI.h"
#include "Utils/CSingleton.h"
#include "serialized_tables.h"
#include <vector>
#include <set>
#include <string>
#include "Defines.h"

class ETechEstimator : public CSingleton<ETechEstimator>
{
    friend class CSingleton<ETechEstimator>;
    ETechEstimator();
    ~ETechEstimator();
	serialized_tables st;
	std::vector<long double> openingsProbas;
	std::set<BWAPI::Unit*> buildingsSeen; 
	std::set<int> buildingsTypesSeen;
	bool notFirstOverlord;
	bool insertBuilding(BWAPI::Unit* u);
	bool insertBuilding(BWAPI::UnitType ut);
    void computeDistribOpenings(int time);
	inline bool testBuildTreePossible(int indBuildTree,
		const std::set<int>& setObs);
public:
	void onUnitDestroy(BWAPI::Unit* u);
	void onUnitShow(BWAPI::Unit* u);
	void onUnitHide(BWAPI::Unit* u);
#ifdef __DEBUG__
	void onFrame();
#endif
};