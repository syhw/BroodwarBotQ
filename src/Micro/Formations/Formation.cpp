#include "Formation.h"

using namespace BWAPI;

Formation::Formation(const Formation& f)
: center(f.center)
, direction(f.direction)
, end_positions(f.end_positions)
, space(30)
{
}

Formation::Formation(const Vec& center, const Vec& direction)
: center(center)
, direction(direction)
, space(30)
{
}

Formation::Formation(const Position& p, const Vec& direction)
: center(p.x(), p.y())
, direction( direction)
, space(30)
{
}

Formation::~Formation()
{
}

// No formations => on one point
void Formation::computeToPositions(const std::vector<pBayesianUnit>& vUnit)
{
	end_positions.clear();
	for (unsigned int i = 0; i < vUnit.size(); i++)
		end_positions.push_back(center.toPosition());
}
