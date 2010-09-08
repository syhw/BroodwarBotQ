#include "ZealotUnit.h"
#include <BWAPI.h>
#include <UnitsGroup.h>

using namespace std;
using namespace BWAPI;

std::set<BWAPI::UnitType> ZealotUnit::setPrio;

ZealotUnit::ZealotUnit(BWAPI::Unit* u, UnitsGroup* ug)
: GroundUnit(u, ug)
{
    if (setPrio.empty())
    {
        setPrio.insert(BWAPI::UnitTypes::Protoss_Reaver);
        //setPrio.insert(BWAPI::UnitTypes::Protoss_High_Templar);
        setPrio.insert(BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode);
        setPrio.insert(BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode);
        setPrio.insert(BWAPI::UnitTypes::Zerg_Hydralisk);
    }
    _fleeingDmg = 12; 
}

ZealotUnit::~ZealotUnit()
{
}

void ZealotUnit::micro()
{
    int hpDiff = _lastTotalHP - (unit->getShields() + unit->getHitPoints());
    if (Broodwar->getFrameCount() - _lastAttackFrame > Broodwar->getLatency() + getAttackDuration())
    {
        if (unit->getGroundWeaponCooldown() == 0)
        {
            updateRangeEnemies();
            updateTargetEnemy();
            attackEnemyUnit(targetEnemy);
        }
        else if (_fleeing || hpDiff >= _fleeingDmg || (unit->getShields() == 0 && hpDiff > 5))
        {
            flee();
        }
        else if (!unit->isMoving() && targetEnemy != NULL)
        {
            //fightMove();
        }
    }
}

void ZealotUnit::check()
{
    if (unit->getUpgradeLevel(UpgradeTypes::Leg_Enhancements) && !setPrio.count(UnitTypes::Terran_Siege_Tank_Siege_Mode))
    {
        setPrio.insert(UnitTypes::Terran_Siege_Tank_Siege_Mode);
        setPrio.insert(BWAPI::UnitTypes::Protoss_High_Templar);
    }
}

int ZealotUnit::getAttackDuration()
{
    return 0;
}

std::set<BWAPI::UnitType> ZealotUnit::getSetPrio()
{
    return ZealotUnit::setPrio;
}