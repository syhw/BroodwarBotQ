#include "DarkTemplarUnit.h"

DarkTemplarUnit::DarkTemplarUnit(BWAPI::Unit* u, UnitsGroup* ug):GroundUnit(u, ug)
{
}

DarkTemplarUnit::~DarkTemplarUnit()
{
}

void DarkTemplarUnit::micro()
{

}

const std::list<BWAPI::UnitType> DarkTemplarUnit::getListePrio() const
{
    return listPriorite;
}
