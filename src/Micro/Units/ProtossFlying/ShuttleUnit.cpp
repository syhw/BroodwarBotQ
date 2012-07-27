#include <PrecompiledHeader.h>
#include "Micro/Units/ProtossFlying/ShuttleUnit.h"

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
	if (unit->getDistance(target) < 51)
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