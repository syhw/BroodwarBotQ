#pragma once

#include <SpecialUnit.h>
#include <BWAPI.h>
#include "MapManager.h"

class HighTemplarUnit : public SpecialUnit
{
protected:
    MapManager* _mapManager;
    int _lastStormFrame;
public:
    HighTemplarUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~HighTemplarUnit();
    std::pair<BWAPI::Position, int> bestStormPos();
    virtual void micro();
    virtual void check();
    virtual std::set<BWAPI::UnitType> getSetPrio();
};