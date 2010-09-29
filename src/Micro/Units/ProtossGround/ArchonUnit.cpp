#include "ArchonUnit.h"
#include <UnitsGroup.h>

std::set<BWAPI::UnitType> ArchonUnit::setPrio;

ArchonUnit::ArchonUnit(BWAPI::Unit* u, UnitsGroup* ug)
: GroundUnit(u, ug)
{
    if (setPrio.empty())
    {
        setPrio.insert(BWAPI::UnitTypes::Zerg_Mutalisk);
        setPrio.insert(BWAPI::UnitTypes::Zerg_Zergling);
        setPrio.insert(BWAPI::UnitTypes::Protoss_High_Templar);
        setPrio.insert(BWAPI::UnitTypes::Protoss_Zealot);
        setPrio.insert(BWAPI::UnitTypes::Terran_Firebat);
        setPrio.insert(BWAPI::UnitTypes::Terran_Marine);
        setPrio.insert(BWAPI::UnitTypes::Terran_Medic);
        setPrio.insert(BWAPI::UnitTypes::Terran_Ghost);
        setPrio.insert(BWAPI::UnitTypes::Terran_Wraith);
    }
}

ArchonUnit::~ArchonUnit()
{
}

bool ArchonUnit::decideToFlee()
{
    if (targetEnemy && targetEnemy->exists() && targetEnemy->isVisible() 
        && Broodwar->getGroundHeight(targetEnemy->getPosition()) > Broodwar->getGroundHeight(_unitPos))
    {
        if (_unitsGroup->nearestChoke && _unitsGroup->nearestChoke->getCenter().getDistance(_unitPos) < 128)
        {
            _fleeing = false;
            return false;
        }
    }
    if (unit->getShields() < 10)
        _fleeingDmg = 12;
    // TODO complete conditions
    int diff = _lastTotalHP - (unit->getShields() + unit->getHitPoints());
    _HPLosts.push_back(diff);
    _sumLostHP += diff;
    if (_HPLosts.size() > 24)
    {
        _sumLostHP -= _HPLosts.front();
        _HPLosts.pop_front();
    }
    if (_sumLostHP > _fleeingDmg)
        _fleeing = true;
    else
        _fleeing = false;
    if (!_fleeing)
    {
        int incDmg = 0;
        for (std::set<Unit*>::const_iterator it = _targetingMe.begin();
            it != _targetingMe.end(); ++it)
        {
            if ((*it)->getDistance(_unitPos) <= (*it)->getType().groundWeapon().maxRange() + 32)
                incDmg += (*it)->getType().groundWeapon().damageAmount() * (*it)->getType().maxGroundHits();
        }
        if (incDmg + _sumLostHP > _fleeingDmg)
            _fleeing = true;
    }
    return _fleeing;
}

void ArchonUnit::micro()
{
    updateTargetingMe();
    decideToFlee();
    int currentFrame = Broodwar->getFrameCount();
    if (currentFrame - _lastAttackFrame <= getAttackDuration()) // not interrupting attacks
        return;
    if (currentFrame - _lastAttackFrame == getAttackDuration() + 1)
        clearDamages();
    if (unit->getGroundWeaponCooldown() <= Broodwar->getLatency() + 1)
    {
        updateRangeEnemies();
        updateTargetEnemy();
        if (!inRange(targetEnemy))
        {
            clearDamages();
        }
        attackEnemyUnit(targetEnemy);
    }
    else if (unit->getGroundWeaponCooldown() > Broodwar->getLatency()*2 + 2) // == (Broodwar->getLatency()+1)*2, safety
    {
        if (!dodgeStorm() && !dragScarab() && !dragMine() && _fleeing)
        {
            simpleFlee();
        }
        else
        {
            fightMove();
        }
    }
}

void ArchonUnit::check()
{
}

int ArchonUnit::getAttackDuration()
{
    return 3+Broodwar->getLatency();
}

std::set<BWAPI::UnitType> ArchonUnit::getSetPrio()
{
    return ArchonUnit::setPrio;
}