#include "LineFormation.h"

using namespace BWAPI;

LineFormation::LineFormation(const LineFormation& f)
: Formation(f) {}

LineFormation::LineFormation(const Vec& center, const Vec& direction)
: Formation(center, direction) {}

LineFormation::LineFormation(const Position& p, const Vec& direction)
: Formation(p, direction) {}

void LineFormation::computeToPositions(const std::vector<pBayesianUnit>& vUnits)
{
	end_positions.clear();
	int maxLength = 0;
	for(std::vector<pBayesianUnit>::const_iterator it = vUnits.begin(); it != vUnits.end(); it++)
	{
		BayesianUnit* bayesianUnit = it->get();
		int length = (abs(direction.x) > abs(direction.y)) ? bayesianUnit->unit->getType().tileHeight() : bayesianUnit->unit->getType().tileWidth();
		if( length > maxLength)
			maxLength = length;
		//lineLength += (abs(direction.x) > abs(direction.y)) ? (*it)->getType().tileHeight() : (*it)->getType().tileWidth();
	}
	maxLength *= TILE_SIZE + 8;
	int lineLength = maxLength * vUnits.size();
	Vec dir( direction.y, direction.x); // rotation of 90° of the normal to obtain director vector.
	Vec begin = center - (dir * lineLength / 2); // Compute the first coord on the line.

	for(unsigned int i = 0; i < vUnits.size(); i++)
		end_positions.push_back( (begin + (dir * i * maxLength)).toPosition());
}
