#include "ReaverUnit.h"

ReaverUnit::ReaverUnit(BWAPI::Unit* u, UnitsGroup* ug):GroundUnit(u, ug)
{
}

ReaverUnit::~ReaverUnit()
{
}

void ReaverUnit::micro()
{
    BWAPI::Broodwar->printf("ReaverUnit::micro non implémenté !");
}

const std::list<BWAPI::UnitType> ReaverUnit::getListePrio() const
{
    return listPriorite;
}

bool ReaverUnit::canHit(BWAPI::Unit* enemy)
{
    BWAPI::Broodwar->printf("ReaverUnit::canHit non implémenté !");
    return false;
}