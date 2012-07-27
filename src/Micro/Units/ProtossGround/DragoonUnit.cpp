#include <PrecompiledHeader.h>
#include "Micro/Units/ProtossGround/DragoonUnit.h"
#include "Micro/UnitsGroup.h"

//#define __SIMPLE_FLEE__

using namespace std;
using namespace BWAPI;

int DragoonUnit::addRange;
int DragoonUnit::attackDuration;

std::set<UnitType> DragoonUnit::setPrio;

DragoonUnit::DragoonUnit(Unit* u)
: GroundUnit(u)
{
    if (Broodwar->self()->getUpgradeLevel(UpgradeTypes::Singularity_Charge))
        addRange = 64;
    else
        addRange = 0;
    attackDuration = 9;

    if (setPrio.empty())
    {
        setPrio.insert(UnitTypes::Terran_Vulture_Spider_Mine);
        setPrio.insert(UnitTypes::Zerg_Lurker);
        setPrio.insert(UnitTypes::Terran_Siege_Tank_Siege_Mode);
        setPrio.insert(UnitTypes::Protoss_Observer);
        setPrio.insert(UnitTypes::Protoss_Carrier);
        setPrio.insert(UnitTypes::Protoss_Archon);
        setPrio.insert(UnitTypes::Protoss_Reaver);
        setPrio.insert(UnitTypes::Terran_Siege_Tank_Tank_Mode);
        setPrio.insert(UnitTypes::Zerg_Defiler);
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
#ifdef __MICRO_DEBUG__
    if (unit->isStartingAttack())
    {
        if (Broodwar->getSelectedUnits().count(unit))
            Broodwar->printf("starting attack at frame: %d, distance to target %f", Broodwar->getFrameCount(), targetEnemy->getDistance(unit));
    }
#endif
    int currentFrame = Broodwar->getFrameCount();
    updateTargetingMe();
    decideToFlee();
    if (currentFrame - _lastAttackFrame <= getAttackDuration()) // not interrupting attacks
        return;
    if (currentFrame - _lastAttackFrame == getAttackDuration() + 1)
        clearDamages();
    /// Dodge storm, drag mine, drag scarab
    if (dodgeStorm() || dragMine() || dragScarab()) 
        return;
    updateRangeEnemies();
    updateTargetEnemy();
    if (unit->getGroundWeaponCooldown() <= Broodwar->getLatencyFrames() + 1)
    {
#ifdef __MICRO_DEBUG__
		Broodwar->drawBoxMap(_unitPos.x()-10, _unitPos.y()-10, _unitPos.x()+8, _unitPos.y()+8, Colors::Red, true);
#endif
        if (!inRange(targetEnemy))
        {
            clearDamages();
        }
        attackEnemyUnit(targetEnemy);
    }
    else if (unit->getGroundWeaponCooldown() > Broodwar->getLatencyFrames() + 2 // (Broodwar->getLatencyFrames()+1)*2, safety TODO check
		|| unit->getGroundWeaponCooldown() == unit->getType().groundWeapon().damageCooldown()) // against really laggy games TODO in other units
    {
		if (currentFrame - _lastClickFrame <= Broodwar->getLatencyFrames() + 3) /// HACK TODO remove/change
			return;                                                             /// HACK TODO remove/change
        if (_fleeing)
        {
#ifdef __SIMPLE_FLEE__
            simpleFlee();
#else
			//if (unit->isStuck()) /// HACK TODO remove/change (unit->isStuck()?)
			//	simpleFlee();
			//else
				flee();
#ifdef __MICRO_DEBUG__
				Broodwar->drawBoxMap(_unitPos.x()-10, _unitPos.y()-10, _unitPos.x()+8, _unitPos.y()+8, Colors::Green, true);
#endif
#endif
        }
        else
        {
#ifdef __MICRO_DEBUG__
		Broodwar->drawBoxMap(_unitPos.x()-10, _unitPos.y()-10, _unitPos.x()+8, _unitPos.y()+8, Colors::Purple, true);
#endif
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
    return Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Singularity_Charge) == 1 ? 192.0 : 128.0;
}

int DragoonUnit::getAttackDuration()
{
    return attackDuration;
}

std::set<UnitType> DragoonUnit::getSetPrio()
{
    return DragoonUnit::setPrio;
}