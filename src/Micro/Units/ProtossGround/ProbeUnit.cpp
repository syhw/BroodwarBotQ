#include <PrecompiledHeader.h>
#include "Micro/Units/ProtossGround/ProbeUnit.h"

ProbeUnit::ProbeUnit(BWAPI::Unit* u)
: GroundUnit(u)
{
}

ProbeUnit::~ProbeUnit()
{
}

void ProbeUnit::micro()
{
    if (Broodwar->getFrameCount() - _lastClickFrame > Broodwar->getLatencyFrames())
    {
        _lastClickFrame = Broodwar->getFrameCount();
        unit->attack(target);
    }
	// TODO complete (back with mineral walk)
}

void ProbeUnit::check()
{
}

int ProbeUnit::getAttackDuration()
{
    return 0;
}

std::set<BWAPI::UnitType> ProbeUnit::getSetPrio()
{
    return std::set<BWAPI::UnitType>();
}
