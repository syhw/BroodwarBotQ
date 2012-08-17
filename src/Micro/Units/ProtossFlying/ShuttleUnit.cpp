#include <PrecompiledHeader.h>
#include "Micro/Units/ProtossFlying/ShuttleUnit.h"
#include "Micro/UnitsGroup.h"

using namespace BWAPI;


ShuttleUnit::ShuttleUnit(Unit* u)
: FlyingUnit(u)
{
}

ShuttleUnit::~ShuttleUnit()
{
}

void ShuttleUnit::micro()
{
	updateTargetingMe();
	decideToFlee();
	/// Emergency drop (not to lose the units)
	if (unit->getShields() < 2 && unit->getHitPoints() < 40)
	{
		for each (Unit* u in unit->getLoadedUnits())
		{
			unit->unload(u);
			_lastClickFrame = Broodwar->getFrameCount();
			return;
		}
	}
	if (Broodwar->getFrameCount() - Broodwar->getLatencyFrames() <= _lastClickFrame)
		return;
	if (unit->getLoadedUnits().empty())
	{
		/*for each (pBayesianUnit bu in _unitsGroup->units)
		{
			if ((bu->getType() == UnitTypes::Protoss_Zealot
				|| bu->getType() == UnitTypes::Protoss_Reaver)
				&& bu->unit->getTarget()->getApproxDistance(bu->unit) > 6*TILE_SIZE)
			{
				_lastClickFrame = Broodwar->getFrameCount();
				unit->load(bu->unit);
			}
		}*/
		for each (Unit* u in unit->getUnitsInRadius(12*TILE_SIZE))
		{
			if ((u->getType() == UnitTypes::Protoss_Zealot
				|| u->getType() == UnitTypes::Protoss_Reaver)
				&& 
				(u->getTarget() != NULL && u->getTarget()->exists() && u->getTarget()->getDistance(u) > 6*TILE_SIZE
				|| _unitsGroup->groupTargetPosition.isValid() && _unitsGroup->groupTargetPosition != Positions::None && u->getDistance(_unitsGroup->groupTargetPosition) > 6*TILE_SIZE))
			{
				_lastClickFrame = Broodwar->getFrameCount();
				unit->load(u);
				break;
			}
		}
	}
	else
	{
		if (unit->getDistance(target) < 2*TILE_SIZE)
		{
			for each (Unit* u in unit->getLoadedUnits())
			{
				unit->unload(u);
				_lastClickFrame = Broodwar->getFrameCount();
				return;
			}
		}
		else
		{
			if (_fleeing)
				flee();
			else
				clickTarget();
		}
	}
}

void ShuttleUnit::check()
{
}

int ShuttleUnit::getAttackDuration()
{
    return 0;
}

std::set<UnitType> ShuttleUnit::getSetPrio()
{
    return std::set<UnitType>();
}