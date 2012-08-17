#include <PrecompiledHeader.h>
#include "Micro/Units/ProtossGround/ProbeUnit.h"
#include "Micro/UnitsGroup.h"

using namespace BWAPI;

Unit* ProbeUnit::backUpMineral = NULL;

ProbeUnit::ProbeUnit(Unit* u)
: GroundUnit(u)
{
}

ProbeUnit::~ProbeUnit()
{
}

bool ProbeUnit::decideToFlee()
{
	updateTargetingMe();
	if (!_targetingMe.empty() &&
		((unit->getShields() < 2 && unit->getHitPoints() > 15) 
		    || (unit->getHitPoints() < 12 // 2 glings attack, sucks against zealots though
			&& (Broodwar->enemy()->getRace() == Races::Protoss || Broodwar->enemy()->getRace() == Races::Zerg))))
		return true;
	return false;
}

void ProbeUnit::micro()
{
	if (Broodwar->getFrameCount() - _lastClickFrame <= Broodwar->getLatencyFrames() + getAttackDuration() + 1)
		return;
	if (dodgeStorm() || dragMine() || dragScarab())
		return;
	if (decideToFlee()) // updates target enemies
	{
		if (backUpMineral != NULL && backUpMineral->exists())
		{
			unit->rightClick(backUpMineral);
			_lastClickFrame = Broodwar->getFrameCount();
		}
		else
			flee();
	}
	else
    {
	    /*_rangeEnemies.clear();
	    for (std::map<Unit*, Position>::const_iterator it 
			= _unitsGroup->enemies.begin(); 
	        it != _unitsGroup->enemies.end(); ++it)
	    {
	        _rangeEnemies.insert(std::make_pair<double, Unit*>(
				_unitPos.getDistance(it->second), it->first));
	    }*/
		updateRangeEnemies();
		if (unit->getGroundWeaponCooldown() > Broodwar->getLatencyFrames() // (Broodwar->getLatencyFrames()+1)*2, safety
			|| unit->getGroundWeaponCooldown() == unit->getType().groundWeapon().damageCooldown()) // against really lag
		{
			if (_rangeEnemies.empty())
				unit->move(_unitsGroup->center);
			else
				unit->move(_rangeEnemies.begin()->second->getPosition());
		}
		else
		{
			_lastAttackFrame = Broodwar->getFrameCount();
			if (_rangeEnemies.empty())
				unit->attack(_unitsGroup->center);
			else
				unit->attack(_rangeEnemies.begin()->second);
		}
		_lastClickFrame = Broodwar->getFrameCount();
	}
}

void ProbeUnit::check()
{
	if (backUpMineral == NULL || !backUpMineral->exists())
	{
		if (Broodwar->getFrameCount() < 10*24*60)
		{
			BWTA::BaseLocation* b = BWTA::getStartLocation(Broodwar->self());
			if (b != NULL && !b->getMinerals().empty())
				backUpMineral = *(b->getMinerals().begin());
			if (backUpMineral == NULL || !backUpMineral->exists())
				backUpMineral = NULL;
		}
	}
}

int ProbeUnit::getAttackDuration()
{
    return 0;
}

std::set<UnitType> ProbeUnit::getSetPrio()
{
    return std::set<UnitType>();
}
