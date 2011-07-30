#include <PrecompiledHeader.h>
#include "Micro/Units/ZergFlying/MutaliskUnit.h"

using namespace BWAPI;

std::set<BWAPI::UnitType> MutaliskUnit::setPrio;

MutaliskUnit::MutaliskUnit(BWAPI::Unit* u)
: FlyingUnit(u)
{
    if (setPrio.empty())
    {
        setPrio.insert(BWAPI::UnitTypes::Protoss_High_Templar);
        setPrio.insert(BWAPI::UnitTypes::Zerg_Scourge);
    }
    _maxDiag = 46; // to space them more (and avoid splash)
}

MutaliskUnit::~MutaliskUnit()
{
    if (Broodwar->getFrameCount() - _lastAttackFrame <= getAttackDuration())
        clearDamages();
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
            if (it->first > ((unit->getType().acceleration() + Broodwar->getLatencyFrames()) * UnitTypes::Zerg_Scourge.topSpeed() // what the scourge may run during my acceleration time + lag
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
    int currentFrame = Broodwar->getFrameCount();    
    if (currentFrame - _lastAttackFrame <= getAttackDuration()) // not interrupting attacks
        return;
    if (currentFrame - _lastAttackFrame == getAttackDuration() + 1)
        clearDamages();
    if (currentFrame - _lastAttackFrame <= getAttackDuration()) // not interrupting attack
        return;

    if (unit->getAirWeaponCooldown() <= Broodwar->getLatencyFrames() + 1)
    {
        updateTargetEnemy();
        if (!inRange(targetEnemy))
        {
            clearDamages();
        }
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

void MutaliskUnit::check()
{
}

int MutaliskUnit::getAttackDuration()
{
    return 1;//Broodwar->getLatencyFrames();
}

std::set<BWAPI::UnitType> MutaliskUnit::getSetPrio()
{
    return MutaliskUnit::setPrio;
}