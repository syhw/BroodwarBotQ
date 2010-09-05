#include "SpecialUnit.h"

SpecialUnit::SpecialUnit(BWAPI::Unit* u, UnitsGroup* ug):BayesianUnit(u, ug)
{
}

SpecialUnit::~SpecialUnit()
{
}

int SpecialUnit::getAttackDuration()
{
    return 3; // TOCHANGE TODO
}