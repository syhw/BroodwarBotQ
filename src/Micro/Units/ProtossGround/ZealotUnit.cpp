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
        setPrio.insert(BWAPI::UnitTypes::Protoss_Zealot);
        setPrio.insert(BWAPI::UnitTypes::Terran_Firebat);
        setPrio.insert(BWAPI::UnitTypes::Zerg_Zergling);
    }
}

ZealotUnit::~ZealotUnit()
{
}

void ZealotUnit::micro()
{
    if (unit->isStartingAttack() || unit->isAttacking())
        return;
    int hpDiff = _lastTotalHP - (unit->getShields() + unit->getHitPoints());
    if (Broodwar->getFrameCount() - _lastAttackOrder > Broodwar->getLatency() + getAttackDuration())
    {
        if (unit->getGroundWeaponCooldown() == 0)
        {
            updateRangeEnemies();
            updateTargetEnemy();
            attackEnemyUnit(targetEnemy);
        }
        else if (_fleeing || hpDiff > 27 || (unit->getShields() == 0 && hpDiff > 13))
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