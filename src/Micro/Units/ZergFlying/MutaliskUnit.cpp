#include "MutaliskUnit.h"

using namespace BWAPI;

std::set<BWAPI::UnitType> MutaliskUnit::setPrio;

MutaliskUnit::MutaliskUnit(BWAPI::Unit* u, UnitsGroup* ug)
: FlyingUnit(u, ug)
{
    if (setPrio.empty())
    {
        setPrio.insert(BWAPI::UnitTypes::Protoss_High_Templar);
        setPrio.insert(BWAPI::UnitTypes::Zerg_Scourge);
    }
    _maxDiag = 46; // to space them more (and avoir splash)
}

MutaliskUnit::~MutaliskUnit()
{
}

bool MutaliskUnit::decideToFlee()
{
    // TODO complete conditions
    return (_lastTotalHP - (unit->getShields() + unit->getHitPoints()) > 30 || (!unit->getShields() && (_lastTotalHP - unit->getHitPoints() > 20)));
}

void MutaliskUnit::micro()
{
    updateRangeEnemies();
    Vec whereFlee = Vec(0, 0);
    for (std::multimap<double, Unit*>::const_iterator it = _rangeEnemies.begin();
        it != _rangeEnemies.end(); ++it)
    {
        if (it->second->isVisible() && it->second->getType() == UnitTypes::Zerg_Scourge && it->second->getTarget() == unit)
        {
            if (it->first > 20) // TOCHANGE
                whereFlee += Vec(it->second->getVelocityX(), it->second->getVelocityY());
        }
    }
    if (whereFlee != Vec(0, 0))
    {
        whereFlee.normalize();
        whereFlee *= _maxDimension;
        whereFlee.translate(_unitPos);
        unit->rightClick(whereFlee.toPosition());
        _lastClickFrame = Broodwar->getFrameCount();
        _lastMoveFrame = Broodwar->getFrameCount();
        _lastRightClick = whereFlee.toPosition();
        return;
    }

    if (targetEnemy != NULL && !(targetEnemy->exists()))
    {
        updateTargetEnemy();
        attackEnemyUnit(targetEnemy);
    }
    else
    {
        if (Broodwar->getFrameCount() - _lastAttackFrame > getAttackDuration())
        {
            if (unit->getGroundWeaponCooldown() == 0)
            {
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

void MutaliskUnit::check()
{
}

int MutaliskUnit::getAttackDuration()
{
    return Broodwar->getLatency();
}

std::set<BWAPI::UnitType> MutaliskUnit::getSetPrio()
{
    return MutaliskUnit::setPrio;
}