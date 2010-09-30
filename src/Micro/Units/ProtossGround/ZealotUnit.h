#pragma once

#include <GroundUnit.h>
#include <BWAPI.h>

class ZealotUnit : public GroundUnit
{
protected:
    static std::set<BWAPI::UnitType> setPrio;
    bool decideToFlee();
    void updateTargetEnemy();
    static std::map<BWAPI::Unit*, int> _zealotsOn;
    void setTargetEnemy(BWAPI::Unit* u);
    void clearDamages();
    int fightMove();
public:
    ZealotUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~ZealotUnit();
    virtual void micro();
    virtual void check();
    virtual int getAttackDuration();
    virtual std::set<BWAPI::UnitType> getSetPrio();
};