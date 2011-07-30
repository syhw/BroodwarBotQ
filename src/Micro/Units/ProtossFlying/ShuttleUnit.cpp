#include <PrecompiledHeader.h>
#include "Micro/Units/ProtossFlying/ShuttleUnit.h"

ShuttleUnit::ShuttleUnit(BWAPI::Unit* u)
: FlyingUnit(u)
{
}

ShuttleUnit::~ShuttleUnit()
{
}

void ShuttleUnit::micro()
{
	if (Broodwar->getFrameCount() - Broodwar->getLatencyFrames() <= _lastClickFrame)
		return;
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
	/// TODO complete
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
		clickTarget();
}

void ShuttleUnit::check()
{
}

int ShuttleUnit::getAttackDuration()
{
    return 0;
}

std::set<BWAPI::UnitType> ShuttleUnit::getSetPrio()
{
    return std::set<BWAPI::UnitType>();
}