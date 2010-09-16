#include "SquareFormation.h"
#include "MapManager.h"
#include <BWTA.h>

using namespace BWAPI;

SquareFormation::SquareFormation(const SquareFormation& f)
: Formation(f) {}

SquareFormation::SquareFormation(const Vec& center, const Vec& direction)
: Formation(center, direction) {}

SquareFormation::SquareFormation(const Position& p, const Vec& direction)
: Formation(p, direction) {}

void SquareFormation::computeToPositions(const std::vector<pBayesianUnit>& vUnits)
{
    BWTA::Region* r = BWTA::getRegion(center.toPosition()); /// DEBUG HACK DEBUG
    center = Vec(r->getCenter().x(), r->getCenter().y());   /// DEBUG HACK DEBUG
	end_positions.clear();
    if (vUnits.size() == 1)
    {
        end_positions.push_back(Position(center.x, center.y));
        return;
    }
    int maxDim = 0;
    for (std::vector<pBayesianUnit>::const_iterator it = vUnits.begin();
        it != vUnits.end(); ++it)
    {
        if ((*it)->getMaxDimension() > maxDim)
            maxDim = (*it)->getMaxDimension();
    }
	unsigned int sizeSide = (unsigned int)sqrt((double)vUnits.size());
	Vec corner = center - Position( (int)(sizeSide/2.0 * (space + maxDim)), (int)(sizeSide/2.0 * (space + maxDim)));
	for( unsigned int i = 0; i < vUnits.size(); i++)
    {
        Position topos = (corner + Position( int(i/sizeSide) * (space + maxDim), (i%sizeSide) * (space + maxDim))).toPosition();
        if (!vUnits[i]->unit->getType().isFlyer() && Broodwar->isWalkable(topos.x()/8, topos.y()/8))
        {
            Position tmp = MapManager::Instance().closestWalkabableSameRegionOrConnected(topos);
            if (tmp != Positions::None)
                topos = tmp;
        }
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
