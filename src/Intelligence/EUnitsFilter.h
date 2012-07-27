#pragma once
#include "Defines.h"
#include "Utils/CSingleton.h"
#include "EViewedUnit.h"
#include <BWTA.h>
#include <BWAPI.h>
#include <map>
#include <utility>

#define ARMY_RADIUS_CLUSTER 10 // in build tiles

// This class works closely tied to MapManager (see MapManager::update())
class EUnitsFilter : public CSingleton<EUnitsFilter>
{
    friend class CSingleton<EUnitsFilter>;
    EUnitsFilter();
    ~EUnitsFilter() { }
	std::map<int, std::list<BWAPI::Unit*> > _eArmies;
    std::set<BWAPI::UnitType> _interestingTypes;
    std::map<BWAPI::Unit*, EViewedUnit> _eViewedUnits;
    std::map<BWAPI::Unit*, std::pair<BWAPI::UnitType, BWAPI::Position> > _invisibleUnits;
public:
    //std::map<BWAPI::Unit*, EViewedUnit> _eViewedUnits;

	inline void updateEArmies();
    void update(BWAPI::Unit* u);
    void filter(BWAPI::Unit* u);
    void onUnitDestroy(BWAPI::Unit* u);
    void onUnitMorph(BWAPI::Unit* u);
    void onUnitShow(BWAPI::Unit* u);
    void onUnitHide(BWAPI::Unit* u);
    void onUnitRenegade(BWAPI::Unit* u);

    const std::map<BWAPI::Unit*, EViewedUnit>& getViewedUnits();
    EViewedUnit getViewedUnit(BWAPI::Unit* u);
    int getNumbersType(BWAPI::UnitType ut);
	BWAPI::Unit* getClosestCenter(BWTA::BaseLocation* b);
    const std::map<BWAPI::Unit*, std::pair<BWAPI::UnitType, BWAPI::Position> >& getInvisibleUnits();
    bool empty();
#ifdef __DEBUG__
    void bwOutput();
#endif
    virtual void update();
};
