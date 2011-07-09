#pragma once
#include <BWTA.h>
#include <BWAPI.h>
#include <vector>
#include "Vec.h"

struct EViewedUnit
{
    // TODO add probability to be in each of the regions.
    BWAPI::Unit* unit;
    BWAPI::UnitType type;
    int HP;
    BWAPI::Position position;
    unsigned int lastSeen;
    Vec velocity;
    EViewedUnit() { }
    EViewedUnit(BWAPI::Unit* u, unsigned int time);
    EViewedUnit(const EViewedUnit& evu);
    void update(unsigned int time);
    ~EViewedUnit() { }
};