#include "ArchonUnit.h"

ArchonUnit::ArchonUnit(BWAPI::Unit* u, UnitsGroup* ug):GroundUnit(u, ug)
{
}

ArchonUnit::~ArchonUnit()
{
}

void ArchonUnit::micro()
{

}

const std::list<BWAPI::UnitType> ArchonUnit::getListePrio() const
{
    return listPriorite;
}