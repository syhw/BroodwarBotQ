#include <PrecompiledHeader.h>
#include "Micro/Units/ProtossFlying/CorsairUnit.h"

using namespace BWAPI;

std::set<BWAPI::UnitType> CorsairUnit::setPrio;

CorsairUnit::CorsairUnit(BWAPI::Unit* u)
: FlyingUnit(u)
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
		if (_fleeing || decideToFlee())
		{
			flee();
		}
		else
		{
			fightMove();
		}
	}
}

void CorsairUnit::check()
{
}

int CorsairUnit::getAttackDuration()
{
    return 2;
}

std::set<BWAPI::UnitType> CorsairUnit::getSetPrio()
{
    return CorsairUnit::setPrio;
}