#pragma once
#include <BWTA.h>
#include <BWAPI.h>
#include <vector>
#include "Vec.h"

struct EViewedUnit
{
    friend std::ostream& operator <<(std::ostream&, const EViewedUnit&);

    BWAPI::Unit* unit;
    int HP;
    BWAPI::Position position;
    int kills;
    unsigned long lastSeen;
    Vec velocity;
    EViewedUnit() { }
    EViewedUnit(BWAPI::Unit* u, unsigned long time);
    EViewedUnit(const EViewedUnit& evu);
    void update(unsigned long time);
    ~EViewedUnit() { }
    //std::ostream& operator <<(std::ostream& os) const;
	const std::istream& operator >>(const std::istream& is) const;
};

std::ostream& operator <<(std::ostream& os, const EViewedUnit& evu);