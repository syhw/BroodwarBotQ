#include <PrecompiledHeader.h>
#include "Micro/Units/ProtossGround/ProbeUnit.h"

ProbeUnit::ProbeUnit(BWAPI::Unit* u, UnitsGroup* ug)
: GroundUnit(u, ug)
{
}

ProbeUnit::~ProbeUnit()
{
}

void ProbeUnit::micro()
{
    if (Broodwar->getFrameCount() - _lastClickFrame > Broodwar->getLatency())
    {
        _lastClickFrame = Broodwar->getFrameCount();
        unit->attack(target);
    }
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
