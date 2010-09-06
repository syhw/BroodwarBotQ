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
    if (vUnit.size() == 1)
    {
        end_positions.push_back(Position(center.x, center.y));
        return;
    }
    int maxDim = 0;
    for (std::vector<pBayesianUnit>::const_iterator it = vUnit.begin();
        it != vUnit.end(); ++it)
    {
        if ((*it)->getMaxDimension() > maxDim)
            maxDim = (*it)->getMaxDimension();
    }
	unsigned int sizeSide = (unsigned int)sqrt((double)vUnit.size());
	Vec corner = center - Position( (int)(sizeSide/2.0 * (space + maxDim)), (int)(sizeSide/2.0 * (space + maxDim)));
	for( unsigned int i = 0; i < vUnit.size(); i++)
    {
        Position topos = (corner + Position( int(i/sizeSide) * (space + maxDim), (i%sizeSide) * (space + maxDim))).toPosition();
        if (topos.isValid())
            end_positions.push_back(topos);
        else
        {
            topos.makeValid();
            end_positions.push_back(topos);
        }
    }
    computeMean();
}
