#include "CorsairUnit.h"

std::set<BWAPI::UnitType> CorsairUnit::setPrio;

CorsairUnit::CorsairUnit(BWAPI::Unit* u, UnitsGroup* ug)
: FlyingUnit(u, ug)
{
    if (setPrio.empty())
    {
        setPrio.insert(BWAPI::UnitTypes::Protoss_Corsair);
        setPrio.insert(BWAPI::UnitTypes::Terran_Wraith);
        setPrio.insert(BWAPI::UnitTypes::Zerg_Scourge);
        setPrio.insert(BWAPI::UnitTypes::Zerg_Mutalisk);
    }
}

CorsairUnit::~CorsairUnit()
{
}

bool CorsairUnit::decideToFlee()
{
    // TODO complete conditions
    return (_lastTotalHP - (unit->getShields() + unit->getHitPoints()) > 30 || (!unit->getShields() && (_lastTotalHP - unit->getHitPoints() > 20)));
}

void CorsairUnit::micro()
{
    if (targetEnemy != NULL && !(targetEnemy->exists()))
    {
        updateRangeEnemies();
        updateTargetEnemy();
        attackEnemyUnit(targetEnemy);
    }
    else
    {
        if (Broodwar->getFrameCount() - _lastAttackFrame > getAttackDuration())
        {
            if (unit->getGroundWeaponCooldown() == 0)
            {
                updateRangeEnemies();
                updateTargetEnemy();
                attackEnemyUnit(targetEnemy);
            }
            else if (_fleeing || decideToFlee())
            {
                flee();
            }
            else if (!unit->isMoving() && targetEnemy != NULL)
            {
                fightMove();
            }
        }
    }
}

void CorsairUnit::check()
{
}

int CorsairUnit::getAttackDuration()
{
    return 0;
}

std::set<BWAPI::UnitType> CorsairUnit::getSetPrio()
{
    return CorsairUnit::setPrio;
}