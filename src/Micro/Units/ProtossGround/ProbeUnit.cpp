#include "ProbeUnit.h"

ProbeUnit::ProbeUnit(BWAPI::Unit* u, UnitsGroup* ug):GroundUnit(u, ug)
{
}

ProbeUnit::~ProbeUnit()
{
}

void ProbeUnit::micro()
{

}

const std::list<BWAPI::UnitType> ProbeUnit::getListePrio() const
{
    return listPriorite;
}
