#include <PrecompiledHeader.h>
#include "Formation.h"

using namespace BWAPI;

Vec closestRegionCenterConnectedToStart(const Vec& c)
{
	double min = DBL_MAX;
	Position tmp(c.toPosition());
	BWTA::Region* s = NULL;
	for each (BWTA::Region* r in BWTA::getRegions())
	{
		double dist = r->getPolygon().getCenter().getApproxDistance(tmp);
		if (dist < min)
		{
			min = dist;
			s = r;
		}
	}
	if (s != NULL)
		return Vec(s->getPolygon().getCenter());
	return c;
}

Formation::Formation(const Formation& f)
: center(f.center)
, direction(f.direction)
, mean(f.center)
, end_positions(f.end_positions)
, space(24)
{
#ifdef __MICRO_DEBUG__
	log("created a formation\n");
#endif
}

Formation::Formation(const Vec& center, int nonFlyers, const Vec& direction)
: center(center)
, direction(direction)
, mean(center)
, space(24)
, non_flyers(nonFlyers)
{
	checkCenterReachability();
    end_positions.clear();
#ifdef __MICRO_DEBUG__
	log("created a formation\n");
#endif
}

Formation::Formation(const Position& p, int nonFlyers, const Vec& direction)
: center(p.x(), p.y())
, direction( direction)
, mean(p.x(), p.y())
, space(24)
, non_flyers(nonFlyers)
{
	checkCenterReachability();
    end_positions.clear();
#ifdef __MICRO_DEBUG__
	log("created a formation\n");
#endif
}

Formation::~Formation()
{
#ifdef __MICRO_DEBUG__
	log("deleted a formation\n");
#endif
}

void Formation::computeMean()
{
    if (!end_positions.size())
        return;
    mean = Vec(0, 0);
    for (std::vector<BWAPI::Position>::const_iterator it = end_positions.begin();
        it != end_positions.end(); ++it)
    {
        mean.x += it->x();
        mean.y += it->y();
    }
    mean /= end_positions.size();
}

void Formation::checkCenterReachability()
{
#ifdef __MICRO_DEBUG__
	Position cp(center.toPosition());
	if (cp == Position(0,0) || cp == Positions::None || cp == Positions::Unknown || !cp.isValid())
		Broodwar->printf("ERROR: given a bad center position for the formation");
#endif
	if (non_flyers && !BWTA::isConnected(TilePosition(center.toPosition()), Broodwar->self()->getStartLocation())) // DropGoal _will_ have 0 nonFlyers
	{
#ifdef __MICRO_DEBUG__
		Broodwar->printf("WARNING: given an unreachable center for the formation");
#endif
		center = closestRegionCenterConnectedToStart(center);
	}
}

// No formations => on one point
void Formation::computeToPositions(const std::vector<pBayesianUnit>& vUnit)
{
	end_positions.clear();
	for (unsigned int i = 0; i < vUnit.size(); i++)
		end_positions.push_back(center.toPosition());
}
