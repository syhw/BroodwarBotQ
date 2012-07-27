#include <PrecompiledHeader.h>
#include "Micro/Units/ProtossFlying/CarrierUnit.h"
#include "Macro/Producer.h"

using namespace BWAPI;

std::set<UnitType> CarrierUnit::setPrio;

CarrierUnit::CarrierUnit(Unit* u)
: FlyingUnit(u)
{
    if (setPrio.empty())
    {
        setPrio.insert(UnitTypes::Protoss_High_Templar);
        setPrio.insert(UnitTypes::Protoss_Corsair);
        setPrio.insert(UnitTypes::Protoss_Scout);
        setPrio.insert(UnitTypes::Protoss_Dragoon);
        setPrio.insert(UnitTypes::Protoss_Archon);
        setPrio.insert(UnitTypes::Protoss_Carrier);
        setPrio.insert(UnitTypes::Terran_Marine);
        setPrio.insert(UnitTypes::Terran_Ghost);
        setPrio.insert(UnitTypes::Terran_Wraith);
        setPrio.insert(UnitTypes::Terran_Valkyrie);
        setPrio.insert(UnitTypes::Terran_Science_Vessel);
        setPrio.insert(UnitTypes::Terran_Battlecruiser);
        setPrio.insert(UnitTypes::Terran_Goliath);
        setPrio.insert(UnitTypes::Zerg_Hydralisk);
        setPrio.insert(UnitTypes::Zerg_Mutalisk);
        setPrio.insert(UnitTypes::Zerg_Devourer);
        setPrio.insert(UnitTypes::Zerg_Scourge);
        setPrio.insert(UnitTypes::Zerg_Defiler);
    }
	_fleeingDmg = 40;
}

CarrierUnit::~CarrierUnit()
{
}

void CarrierUnit::micro()
{ 
	_mode = MODE_FIGHT_A;
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
	decideToFlee();

	if (unit->getInterceptorCount() < 8)
	{
		unit->train(UnitTypes::Protoss_Interceptor);
#ifndef __MICRO_PROJECT__
		if (!Broodwar->self()->getUpgradeLevel(UpgradeTypes::Carrier_Capacity))
			TheProducer->researchUpgrade(UpgradeTypes::Carrier_Capacity);
#endif
	}
	
	int currentFrame = Broodwar->getFrameCount();
    if (currentFrame - _lastAttackFrame <= getAttackDuration()) // not interrupting attack
        return;
    if (unit->getAirWeaponCooldown() <= Broodwar->getLatencyFrames() + 1)
    {
        updateTargetEnemy();
        attackEnemyUnit(targetEnemy);
    }
    else if (unit->getAirWeaponCooldown() > Broodwar->getLatencyFrames() + 2) 
	{
		if (currentFrame - _lastClickFrame <= Broodwar->getLatencyFrames() + 3) /// HACK TODO remove/change
			return;  
		if (_fleeing)
		{
			flee();
		}
		else
		{
			fightMove();
		}
	}
}

void CarrierUnit::check()
{
}

int CarrierUnit::getAttackDuration()
{
    return 9;
}

std::set<UnitType> CarrierUnit::getSetPrio()
{
    return CarrierUnit::setPrio;
}