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
        setPrio.insert(BWAPI::UnitTypes::Protoss_High_Templar);
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
    if (Broodwar->getFrameCount() - _lastAttackOrder > getAttackDuration())
    {
        if (unit->getGroundWeaponCooldown() == 0)
        {
            updateRangeEnemies();
            updateTargetEnemy();
            attackEnemyUnit(targetEnemy);
        }
        else if (_fleeing || _lastTotalHP - (unit->getShields() + unit->getHitPoints()) > 0)
        {
            //flee();
        }
        else if (!unit->isMoving() && targetEnemy != NULL)
        {
            //fightMove();
        }
    }
}

void ZealotUnit::check()
{
}

int ZealotUnit::getAttackDuration()
{
    return 0;
}

std::set<BWAPI::UnitType> ZealotUnit::getSetPrio()
{
    return ZealotUnit::setPrio;
}