#include <PrecompiledHeader.h>
#include "Micro/Units/ProtossSpecial/ArbiterUnit.h"
#include "Macro/Producer.h"
#include "Micro/UnitsGroup.h"

std::set<BWAPI::UnitType> ArbiterUnit::setPrio;

ArbiterUnit::ArbiterUnit(BWAPI::Unit* u)
: FlyingUnit(u)
{
    if (setPrio.empty())
    {
        setPrio.insert(BWAPI::UnitTypes::Protoss_High_Templar);
        setPrio.insert(BWAPI::UnitTypes::Protoss_Dragoon);
        setPrio.insert(BWAPI::UnitTypes::Protoss_Scout);
        setPrio.insert(BWAPI::UnitTypes::Terran_Wraith);
        setPrio.insert(BWAPI::UnitTypes::Terran_Vulture_Spider_Mine);
        setPrio.insert(BWAPI::UnitTypes::Zerg_Hydralisk);
        setPrio.insert(BWAPI::UnitTypes::Zerg_Defiler);
        setPrio.insert(BWAPI::UnitTypes::Zerg_Mutalisk);
    }
	_fleeingDmg = 28;
}

ArbiterUnit::~ArbiterUnit()
{
}

void ArbiterUnit::micro()
{ 
	updateRangeEnemies();
    Vec whereFlee = Vec(0, 0);
	for (std::multimap<double, BWAPI::Unit*>::const_iterator it = _rangeEnemies.begin();
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
	decideToFlee();

#ifndef __MICRO_PROJECT__
	if (!Broodwar->self()->hasResearched(TechTypes::Stasis_Field))
		TheProducer->researchTech(TechTypes::Stasis_Field);
#endif

	int currentFrame = Broodwar->getFrameCount();
    if (currentFrame - _lastAttackFrame <= getAttackDuration()) // not interrupting attack
        return;
	if (currentFrame - _lastClickFrame <= Broodwar->getLatencyFrames() + 3) /// HACK TODO remove/change
		return;    
	if (unit->getEnergy() >= 100 && _rangeEnemies.size() > 5)
	{
		for (std::multimap<double, BWAPI::Unit*>::const_reverse_iterator it = _rangeEnemies.rbegin(); // lol
			it != _rangeEnemies.rend(); ++it)
		{
			if (it->first < 9*TILE_SIZE)
			{
				//unit->useTech(TechTypes::Stasis_Field, it->second);
				unit->useTech(TechTypes::Stasis_Field, it->second->getPosition());
				_lastClickFrame = Broodwar->getFrameCount();
				return;
			}
		}
	}
    if (unit->getAirWeaponCooldown() <= Broodwar->getLatencyFrames() + 1)
    {
        updateTargetEnemy();
        attackEnemyUnit(targetEnemy);
    } 
	else if (unit->getAirWeaponCooldown() > Broodwar->getLatencyFrames() + 2) 
	{	
		if (_fleeing)
		{
			flee();
		}
		else
		{
			//fightMove();
			unit->move(_unitsGroup->center);
			_lastClickFrame = Broodwar->getFrameCount();
		}
	}
}

void ArbiterUnit::check()
{
}

int ArbiterUnit::getAttackDuration()
{
    return 7;
}

std::set<BWAPI::UnitType> ArbiterUnit::getSetPrio()
{
    return ArbiterUnit::setPrio;
}