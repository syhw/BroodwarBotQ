#include "SquareFormation.h"

using namespace BWAPI;

SquareFormation::SquareFormation(const SquareFormation& f)
: Formation(f) {}

SquareFormation::SquareFormation(const Vec& center, const Vec& direction)
: Formation(center, direction) {}

SquareFormation::SquareFormation(const Position& p, const Vec& direction)
: Formation(p, direction) {}

void SquareFormation::computeToPositions(const std::vector<pBayesianUnit>& vUnit)
{
	end_positions.clear();

	unsigned int sizeSide = (unsigned int)sqrt( (double)vUnit.size());
	Vec corner = center - Position( (int)(sizeSide/2.0 * space), (int)(sizeSide/2.0 * space));
	for( unsigned int i = 0; i < vUnit.size(); i++)
    {
        Position topos = (corner + Position( int(i/sizeSide) * space, (i%sizeSide) * space)).toPosition();
        if (topos.isValid())
            end_positions.push_back(topos);
        else
        {
            topos.makeValid();
            end_positions.push_back(topos);
        }
    }
}
