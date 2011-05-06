#include <PrecompiledHeader.h>
#include "SpecialUnit.h"

ProbTables SpecialUnit::_sProbTables = ProbTables();

SpecialUnit::SpecialUnit(BWAPI::Unit* u, UnitsGroup* ug)
: BayesianUnit(u, ug, &_sProbTables)
{
}

SpecialUnit::~SpecialUnit()
{
}

int SpecialUnit::getAttackDuration()
{
    return 1; // TOCHANGE TODO
}