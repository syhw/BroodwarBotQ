#include <PrecompiledHeader.h>
#include "Micro/Units/ProtossGround/DragoonUnit.h"
#include "Micro/UnitsGroup.h"

//#define __SIMPLE_FLEE__

using namespace std;
using namespace BWAPI;

int DragoonUnit::addRange;
int DragoonUnit::attackDuration;

std::set<BWAPI::UnitType> DragoonUnit::setPrio;

DragoonUnit::DragoonUnit(BWAPI::Unit* u)
: GroundUnit(u)
{
    if (Broodwar->self()->getUpgradeLevel(UpgradeTypes::Singularity_Charge))
        addRange = 64;
    else
        addRange = 0;
    attackDuration = 9;

    if (setPrio.empty())
    {
        setPrio.insert(BWAPI::UnitTypes::Terran_Vulture_Spider_Mine);
        setPrio.insert(BWAPI::UnitTypes::Zerg_Lurker);
        setPrio.insert(BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode);
        setPrio.insert(BWAPI::UnitTypes::Protoss_Observer);
        setPrio.insert(BWAPI::UnitTypes::Protoss_Carrier);
        setPrio.insert(BWAPI::UnitTypes::Protoss_Archon);
        setPrio.insert(BWAPI::UnitTypes::Protoss_Reaver);
        setPrio.insert(BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode);
        setPrio.insert(BWAPI::UnitTypes::Zerg_Defiler);
    }
    _fleeingDmg = 22; // 
}

DragoonUnit::~DragoonUnit()
{
    if (Broodwar->getFrameCount() - _lastAttackFrame <= getAttackDuration())
        clearDamages();
}

int DragoonUnit::addRangeGround()
{
    return addRange;
}

int DragoonUnit::addRangeAir()
{
    return addRange;
}

bool DragoonUnit::decideToFlee()
{
    if (targetEnemy && targetEnemy->exists() && targetEnemy->isVisible() 
        && Broodwar->getGroundHeight(TilePosition(targetEnemy->getPosition())) > Broodwar->getGroundHeight(TilePosition(_unitPos)))
    {
        if (_unitsGroup && _unitsGroup->nearestChoke && _unitsGroup->nearestChoke->getCenter().getDistance(_unitPos) < 128)
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

void DragoonUnit::micro()
{
#ifdef __DEBUG__
    if (unit->isStartingAttack())
    {
        if (Broodwar->getSelectedUnits().count(unit))
            Broodwar->printf("starting attack at frame: %d, distance to target %f", Broodwar->getFrameCount(), targetEnemy->getDistance(unit));
    }
#endif
    updateTargetingMe();
    decideToFlee();
    int currentFrame = Broodwar->getFrameCount();
    if (currentFrame - _lastAttackFrame <= getAttackDuration()) // not interrupting attacks
        return;
    if (currentFrame - _lastAttackFrame == getAttackDuration() + 1)
        clearDamages();
    /// Dodge storm, drag mine, drag scarab
    if (dodgeStorm() || dragMine() || dragScarab()) 
        return;
    if (unit->getGroundWeaponCooldown() <= Broodwar->getLatencyFrames() + 1)
    {
        updateRangeEnemies();
        updateTargetEnemy();
        if (!inRange(targetEnemy))
        {
            clearDamages();
        }
        attackEnemyUnit(targetEnemy);
    }
    else if (unit->getGroundWeaponCooldown() > Broodwar->getLatencyFrames()*2 + 2) // == (Broodwar->getLatencyFrames()+1)*2, safety
    {
        if (!dodgeStorm() && !dragScarab() && !dragMine() && _fleeing)
        {
#ifdef __SIMPLE_FLEE__
            simpleFlee();
#else
			flee();
#endif
        }
        else
        {
            fightMove();
        }
    }
}

void DragoonUnit::check()
{
    if (Broodwar->self()->getUpgradeLevel(UpgradeTypes::Singularity_Charge))
        addRange = 64;
}

double DragoonUnit::getMaxRange()
{
    return BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Singularity_Charge) == 1 ? 192.0 : 128.0;
}

int DragoonUnit::getAttackDuration()
{
    return attackDuration;
}

std::set<BWAPI::UnitType> DragoonUnit::getSetPrio()
{
    return DragoonUnit::setPrio;
}