#pragma once

#include "Micro/Units/GroundUnit.h"
#include <BWAPI.h>

class ProbeUnit : public GroundUnit
{
	static BWAPI::Unit* backUpMineral;
	bool decideToFlee();
public:
    ProbeUnit(BWAPI::Unit* u);
    virtual ~ProbeUnit();
    virtual void micro();
    virtual void check();
    virtual int getAttackDuration();
    virtual std::set<BWAPI::UnitType> getSetPrio();
};