#pragma once

#include <GroundUnit.h>
#include <BWAPI.h>

class DragoonUnit : public GroundUnit
{
protected:
    std::list<BWAPI::UnitType> listPriorite;
    void dragoonIA(std::set<BWAPI::Unit*> enemies, double maxRangeGoonEnemy);
public:
    DragoonUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~DragoonUnit();
    virtual void micro();
    virtual const std::list<BWAPI::UnitType> getListePrio() const;
    virtual bool canHit(BWAPI::Unit* enemy);
    static double getMaxRange();
};