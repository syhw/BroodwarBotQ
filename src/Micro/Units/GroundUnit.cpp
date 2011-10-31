#include <PrecompiledHeader.h>
#include "GroundUnit.h"

ProbTables GroundUnit::_sProbTables = ProbTables(-1); // -1 for ground

GroundUnit::GroundUnit(BWAPI::Unit* u) // Default ProbTables for GroundUnits
: BayesianUnit(u, &_sProbTables)
{
}

GroundUnit::GroundUnit(BWAPI::Unit* u, // More specialized ProbTables
					   const ProbTables* probTables)
: BayesianUnit(u, probTables)
{
}

GroundUnit::~GroundUnit()
{
}