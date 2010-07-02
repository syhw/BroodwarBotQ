#include "SpecialUnit.h"

SpecialUnit::SpecialUnit(BWAPI::Unit* u, UnitsGroup* ug):BayesianUnit(u, ug)
{
}

SpecialUnit::~SpecialUnit()
{
}

int SpecialUnit::damagesOn(BWAPI::Unit* enemy)
{
    return 0;
}