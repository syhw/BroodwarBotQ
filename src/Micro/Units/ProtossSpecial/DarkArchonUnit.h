#pragma once

#include "Micro/Units/SpecialUnit.h"
#include <BWAPI.h>

class DarkArchonUnit : public SpecialUnit
{
	int _lastCastFrame;
	static std::set<BWAPI::UnitType> casters;
	static std::set<BWAPI::UnitType> setPrio;
public:
    DarkArchonUnit(BWAPI::Unit* u);
    virtual ~DarkArchonUnit();
    virtual void micro();
    virtual void check();
    virtual int getAttackDuration();
    virtual std::set<BWAPI::UnitType> getSetPrio();
};