#include "ReaverUnit.h"

ReaverUnit::ReaverUnit(BWAPI::Unit* u, UnitsGroup* ug):GroundUnit(u, ug)
{
}

ReaverUnit::~ReaverUnit()
{
}

void ReaverUnit::micro()
{

}

const std::list<BWAPI::UnitType> ReaverUnit::getListePrio() const
{
    return listPriorite;
}
