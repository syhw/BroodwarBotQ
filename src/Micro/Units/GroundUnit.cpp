#include <PrecompiledHeader.h>
#include "GroundUnit.h"

ProbTables GroundUnit::_sProbTables = ProbTables(-1); // -1 for ground

GroundUnit::GroundUnit(BWAPI::Unit* u, BasicUnitsGroup* ug)
: BayesianUnit(u, ug, &_sProbTables)
{
}

GroundUnit::GroundUnit(BWAPI::Unit* u, BasicUnitsGroup* ug,
					   const ProbTables* probTables)
: BayesianUnit(u, ug, probTables)
{
}

GroundUnit::~GroundUnit()
{
}