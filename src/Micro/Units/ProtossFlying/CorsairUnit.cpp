#include "CorsairUnit.h"

using namespace BWAPI;

std::set<BWAPI::UnitType> CorsairUnit::setPrio;

CorsairUnit::CorsairUnit(BWAPI::Unit* u, UnitsGroup* ug)
: FlyingUnit(u, ug)
{
    if (setPrio.empty())
    {
        setPrio.insert(BWAPI::UnitTypes::Protoss_Corsair);
        setPrio.insert(BWAPI::UnitTypes::Terran_Wraith);
        setPrio.insert(BWAPI::UnitTypes::Zerg_Scourge);
    }
    if (Broodwar->enemy()->getRace() == Races::Zerg)
        _maxDiag = 46; // to space them more (and avoid splash)
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
    _fleeing = decideToFlee();
    if (Broodwar->getFrameCount() - _lastAttackFrame <= getAttackDuration()) // not interrupting attack
        return;
    if (unit->getGroundWeaponCooldown() == 0)
    {
        updateTargetEnemy();
        attackEnemyUnit(targetEnemy);
    }
    else if (_fleeing)
    {
        flee();
    }
    else
    {
        fightMove();
    }
}

void CorsairUnit::check()
{
}

int CorsairUnit::getAttackDuration()
{
    return Broodwar->getLatency();
}

std::set<BWAPI::UnitType> CorsairUnit::getSetPrio()
{
    return CorsairUnit::setPrio;
}