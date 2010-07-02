#include "ProbeUnit.h"

ProbeUnit::ProbeUnit(BWAPI::Unit* u, UnitsGroup* ug):GroundUnit(u, ug)
{
}

ProbeUnit::~ProbeUnit()
{
}

void ProbeUnit::micro()
{
    BWAPI::Broodwar->printf("ProbeUnit::micro non implémenté !");
}

const std::list<BWAPI::UnitType> ProbeUnit::getListePrio() const
{
    return listPriorite;
}

bool ProbeUnit::canHit(BWAPI::Unit* enemy)
{
    BWAPI::Broodwar->printf("ProbeUnit::canHit non implémenté !");
    return false;
}