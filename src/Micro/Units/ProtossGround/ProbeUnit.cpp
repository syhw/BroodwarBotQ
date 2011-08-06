#include <PrecompiledHeader.h>
#include "Micro/Units/ProtossGround/ProbeUnit.h"
#include "Micro/UnitsGroup.h"

BWAPI::Unit* ProbeUnit::backUpMineral = NULL;

ProbeUnit::ProbeUnit(BWAPI::Unit* u)
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
		(unit->getShields() < 2 || unit->getHitPoints() < 6))
		return true;
	return false;
}

void ProbeUnit::micro()
{
	if (Broodwar->getFrameCount() - _lastClickFrame <= Broodwar->getLatencyFrames() + getAttackDuration())
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
        _lastClickFrame = Broodwar->getFrameCount();
		unit->attack(_unitsGroup->center);
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
    return 1;
}

std::set<BWAPI::UnitType> ProbeUnit::getSetPrio()
{
    return std::set<BWAPI::UnitType>();
}
