#pragma once
#include <BWTA.h>
#include <BWAPI.h>
#include <vector>
#include "Vec.h"

struct EViewedUnit
{
    BWAPI::Unit* unit;
    int HP;
    BWAPI::Position position;
    int kills;
    unsigned long lastSeen;
    Vec velocity;
    EViewedUnit() { }
    EViewedUnit(BWAPI::Unit* u, unsigned long time);
    void update(unsigned long time);
    ~EViewedUnit() { }
};