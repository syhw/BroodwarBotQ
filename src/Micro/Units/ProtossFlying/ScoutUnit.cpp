#include <PrecompiledHeader.h>
#include "Micro/Units/ProtossFlying/ScoutUnit.h"

std::set<BWAPI::UnitType> ScoutUnit::setPrio;

ScoutUnit::ScoutUnit(BWAPI::Unit* u,UnitsGroup* ug)
: FlyingUnit(u, ug)
{
    if (setPrio.empty())
    {
        setPrio.insert(BWAPI::UnitTypes::Protoss_Corsair);
        setPrio.insert(BWAPI::UnitTypes::Protoss_Scout);
        setPrio.insert(BWAPI::UnitTypes::Protoss_Carrier);
        setPrio.insert(BWAPI::UnitTypes::Terran_Wraith);
        setPrio.insert(BWAPI::UnitTypes::Terran_Valkyrie);
        setPrio.insert(BWAPI::UnitTypes::Terran_Science_Vessel);
        setPrio.insert(BWAPI::UnitTypes::Terran_Battlecruiser);
        setPrio.insert(BWAPI::UnitTypes::Zerg_Scourge);
        setPrio.insert(BWAPI::UnitTypes::Zerg_Mutalisk);
        setPrio.insert(BWAPI::UnitTypes::Zerg_Devourer);
    }
}

ScoutUnit::~ScoutUnit()
{
}

bool ScoutUnit::decideToFlee()
{
    // TODO complete conditions
    return (_lastTotalHP - (unit->getShields() + unit->getHitPoints()) > 30 || (!unit->getShields() && (_lastTotalHP - unit->getHitPoints() > 20)));
}

void ScoutUnit::micro()
{
    updateRangeEnemies();
    Vec whereFlee = Vec(0, 0);
    for (std::multimap<double, Unit*>::const_iterator it = _rangeEnemies.begin();
        it != _rangeEnemies.end(); ++it)
    {
        if (it->second->isVisible() && it->second->getType() == UnitTypes::Zerg_Scourge && it->second->getTarget() == unit)
        {
            if (it->first > ((unit->getType().acceleration() + Broodwar->getLatency()) * UnitTypes::Zerg_Scourge.topSpeed() // what the scourge may run during my acceleration time + lag
                - unit->getType().acceleration() * (unit->getType().topSpeed()/2.1) // what I may run during my acceleration time
                + _maxDimension/2 - UnitTypes::Zerg_Scourge.dimensionUp() + 0.1))// difference of both sizes
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
    if (Broodwar->getFrameCount() - _lastAttackFrame <= getAttackDuration()) // not interrupting attack
        return;
    if (unit->getAirWeaponCooldown() <= Broodwar->getLatency() + 1)
    {
        updateTargetEnemy();
        attackEnemyUnit(targetEnemy);
    }
    else if (_fleeing || decideToFlee())
    {
        flee();
    }
    else
    {
        fightMove();
    }
}

void ScoutUnit::check()
{
}

int ScoutUnit::getAttackDuration()
{
    return Broodwar->getLatency();
}

std::set<BWAPI::UnitType> ScoutUnit::getSetPrio()
{
    return ScoutUnit::setPrio;
}