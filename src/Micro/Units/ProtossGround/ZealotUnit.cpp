#include <PrecompiledHeader.h>
#include "Micro/Units/ProtossGround/ZealotUnit.h"
#include "Micro/UnitsGroup.h"
#include "Regions/MapManager.h"

using namespace std;
using namespace BWAPI;

ProbTables ZealotUnit::_sProbTables = ProbTables(BWAPI::UnitTypes::Protoss_Zealot.getID());

std::set<BWAPI::UnitType> ZealotUnit::setPrio;

ZealotUnit::ZealotUnit(BWAPI::Unit* u)
: GroundUnit(u, &_sProbTables)
{
    if (setPrio.empty())
    {
        setPrio.insert(BWAPI::UnitTypes::Protoss_Reaver);
        setPrio.insert(BWAPI::UnitTypes::Protoss_High_Templar);
        setPrio.insert(BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode);
        setPrio.insert(BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode);
        setPrio.insert(BWAPI::UnitTypes::Zerg_Hydralisk);
		setPrio.insert(BWAPI::UnitTypes::Zerg_Lurker);
		setPrio.insert(BWAPI::UnitTypes::Zerg_Defiler);
    }
    _fleeingDmg = 36; // one round of storm = 14
}

ZealotUnit::~ZealotUnit()
{
}

#ifdef __LEARNING_PROB_TABLES__
void ZealotUnit::initProbTables()
{
	_sProbTables = ProbTables(BWAPI::UnitTypes::Protoss_Zealot.getID());
}
#endif

int ZealotUnit::fightMove()
{
	if (Broodwar->getFrameCount() - _lastClickFrame <= Broodwar->getLatencyFrames())
		return 0;
    /// approach our target enemy if not in range and in our priority targets
    if (targetEnemy != NULL && targetEnemy->exists() && targetEnemy->isVisible() && targetEnemy->isDetected()
        && setPrio.count(targetEnemy->getType())
        && (targetEnemy->getDistance(_unitPos) > 45.0 || !inRange(targetEnemy)))
    {
        unit->move(targetEnemy->getPosition());
        _lastClickFrame = Broodwar->getFrameCount();
        _lastMoveFrame = Broodwar->getFrameCount();
        _fightMoving = true;
        return 1;
    }
    double dist = target.getDistance(_unitPos);
    /// approach out of range target
    if (oorTargetEnemy != NULL && oorTargetEnemy->exists() && oorTargetEnemy->isVisible() && targetEnemy->isDetected()
        && !inRange(oorTargetEnemy))
    {
        unit->move(oorTargetEnemy->getPosition());
        _lastClickFrame = Broodwar->getFrameCount();
        _lastMoveFrame = Broodwar->getFrameCount();
        _fightMoving = true;
        return 2;
    }
    return 0;
}

bool ZealotUnit::decideToFlee()
{
	_fleeing = false;
	if (unit->getShields() < 5 && unit->isUnderAttack()
		&& !(targetEnemy && targetEnemy->exists() && targetEnemy->isVisible() && targetEnemy->isDetected()
		&& setPrio.count(targetEnemy->getType())))
		_fleeing = true;
    return _fleeing;
}

void ZealotUnit::updateTargetEnemy()
{
	/// oorTarget = closest in the setPrio not in range
	oorTargetEnemy = NULL;
	double closest = DBL_MAX;
	for (map<Unit*, Position>::const_iterator it = _unitsGroup->enemies.begin();
		it != _unitsGroup->enemies.end(); ++it)
	{
        if (it->first->exists() && it->first->isVisible() && it->first->isDetected()
			&& setPrio.count(it->first->getType())
			&& it->first->getDistance(_unitPos) < closest)
		{
			closest = it->first->getDistance(_unitPos);
			oorTargetEnemy = it->first;
		}
	}
	/// targetEnemy = in range, the ones in setPrio have the priority
	/// otherwise the closest
	targetEnemy = NULL;
	Unit* closestEnemy = NULL;
	closest = DBL_MAX;
	for (map<Unit*, Position>::const_iterator it = _unitsGroup->enemies.begin();
		it != _unitsGroup->enemies.end(); ++it)
	{
		/// Rule out what we don't want to attack
        if (!it->first->exists() || !it->first->isVisible() || !it->first->isDetected())
            continue;
        if (inRange(it->first))
            continue;
        UnitType testType = it->first->getType();
        if (testType.isBuilding() 
            && testType != BWAPI::UnitTypes::Protoss_Photon_Cannon
			&& (testType != BWAPI::UnitTypes::Terran_Bunker || !it->first->isAttacking())
            && testType != BWAPI::UnitTypes::Terran_Missile_Turret
            && testType != BWAPI::UnitTypes::Zerg_Sunken_Colony
            && testType != BWAPI::UnitTypes::Zerg_Spore_Colony)
            continue;
		/// Take one in the setPrio and in range or one in range or the closest
		if (inRange(it->first))
		{
			targetEnemy = it->first;
			if (setPrio.count(it->first->getType()))
				break;
		} else
		{
			if (it->first->getDistance(_unitPos) < closest)
			{
				closest = it->first->getDistance(_unitPos);
				closestEnemy = it->first;
			}
		}
	}
	if (targetEnemy == NULL)
		targetEnemy = closestEnemy;
}


void ZealotUnit::micro()
{
    int currentFrame = Broodwar->getFrameCount();
    if (unit->isStartingAttack())
        _lastAttackFrame = currentFrame;
	if (currentFrame - _lastClickFrame <= Broodwar->getLatencyFrames())
		return;
    if (currentFrame - _lastAttackFrame <= getAttackDuration()) // not interrupting attacks
        return;
    updateTargetingMe();
    /// Dodge storm, drag mine, drag scarab
    if (dodgeStorm() || dragMine() || dragScarab()) 
        return;
    decideToFlee();

    if (unit->getGroundWeaponCooldown() <= Broodwar->getLatencyFrames() + 2)
    {
        updateTargetEnemy();
		if (targetEnemy != NULL && targetEnemy->exists() && targetEnemy->isVisible() && targetEnemy->isDetected()) 
		{
			// attack enemy unit
			unit->rightClick(targetEnemy);
			_lastClickFrame = Broodwar->getFrameCount();
			_lastAttackFrame = Broodwar->getFrameCount();
		}
		else
			unit->attack(_unitsGroup->enemiesCenter);
    }
    else if (unit->getGroundWeaponCooldown() > Broodwar->getLatencyFrames()*2 + 2) // == (Broodwar->getLatencyFrames()+1)*2, safety
    {
        if (_fleeing)
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

void ZealotUnit::check()
{
    if (unit->getUpgradeLevel(UpgradeTypes::Leg_Enhancements) && !setPrio.count(UnitTypes::Terran_Siege_Tank_Siege_Mode))
    {
        setPrio.insert(BWAPI::UnitTypes::Protoss_High_Templar);
    }
}

int ZealotUnit::getAttackDuration()
{
    return 1;
}

std::set<BWAPI::UnitType> ZealotUnit::getSetPrio()
{
    return ZealotUnit::setPrio;
}