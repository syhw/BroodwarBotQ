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

bool SpecialUnit::withinRange(BWAPI::Unit* enemy)
{
    return unit->getDistance(enemy) < 1.0; // to be confirmed
}