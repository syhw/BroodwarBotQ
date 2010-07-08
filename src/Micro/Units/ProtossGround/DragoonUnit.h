#pragma once

#include <GroundUnit.h>
#include <BWAPI.h>

class DragoonUnit : public GroundUnit
{
protected:
    void dragoonIA(std::set<BWAPI::Unit*> enemies, double maxRangeGoonEnemy);
public:
    static BWAPI::UnitType listPriorite[NUMBER_OF_PRIORITY];
    DragoonUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~DragoonUnit();
    virtual void micro();
    virtual bool canHit(BWAPI::Unit* enemy);
    static double getMaxRange();
    virtual int getTimeToAttack();
};