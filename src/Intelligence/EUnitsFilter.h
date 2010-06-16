#pragma once
#include "CSingleton.h"
#include "TimeManager.h"
#include "EViewedUnit.h"
#include <BWTA.h>
#include <BWAPI.h>
#include <map>
#include <utility>

class EUnitsFilter : public CSingleton<EUnitsFilter> 
{
    TimeManager* timeManager;
    std::map<BWAPI::Unit*, EViewedUnit> eViewedUnits;
public:
    EUnitsFilter();
    ~EUnitsFilter() { }
    void update(BWAPI::Unit* u);
	virtual void onUnitDestroy(BWAPI::Unit* u);
	virtual void onUnitMorph(BWAPI::Unit* u);
	virtual void onUnitShow(BWAPI::Unit* u);
	virtual void onUnitHide(BWAPI::Unit* u);
	virtual void onUnitRenegade(BWAPI::Unit* u);
};