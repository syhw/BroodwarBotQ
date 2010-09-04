#pragma once

#include <SpecialUnit.h>
#include <BWAPI.h>

class ArbiterUnit : public SpecialUnit
{
protected:
    static std::set<BWAPI::UnitType> setPrio;
public:
    ArbiterUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~ArbiterUnit();
    virtual void micro();
    virtual void check();
    virtual bool canHit(BWAPI::Unit* enemy);
    virtual int getTimeToAttack();
    virtual std::set<BWAPI::UnitType> getSetPrio();
};