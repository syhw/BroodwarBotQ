#pragma once
#include "Defines.h"
#include "CSingleton.h"
#include "BaseObject.h"
#include "TimeManager.h"
#include "EViewedUnit.h"
#include <BWTA.h>
#include <BWAPI.h>
#include <map>
#include <utility>
#include "Data.h"

#ifdef BW_QT_DEBUG
#include "QtOutputer.h"
#endif

class EUnitsFilter : public CSingleton<EUnitsFilter> 
{
    TimeManager* timeManager;
    std::map<BWAPI::Unit*, EViewedUnit> eViewedUnits;
public:
    EUnitsFilter();
    ~EUnitsFilter() { }
    void update(BWAPI::Unit* u);
	void onUnitDestroy(BWAPI::Unit* u);
	void onUnitMorph(BWAPI::Unit* u);
	void onUnitShow(BWAPI::Unit* u);
	void onUnitHide(BWAPI::Unit* u);
	void onUnitRenegade(BWAPI::Unit* u);
#ifdef BW_QT_DEBUG
    QtOutputer qtOutputer;
#endif
};