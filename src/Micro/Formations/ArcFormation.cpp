#include "ArcFormation.h"
#include "MapManager.h"

using namespace BWAPI;

ArcFormation::ArcFormation(const ArcFormation& f)
: Formation(f) 
{}

ArcFormation::ArcFormation(const Vec& onCircle, const Vec& circleCenter)
: Formation(center, direction) {}

ArcFormation::ArcFormation(const Position& onCircle, const Position& circleCenter)
: Formation(onCircle, Vec(circleCenter.x(), circleCenter.y())) {}

void ArcFormation::computeToPositions(const std::vector<pBayesianUnit>& vUnits)
{
	end_positions.clear();
    if (!vUnits.size())
        return;
    if (vUnits.size() == 1)
    {
        end_positions.push_back(Position(center.x, center.y));
        return;
    }
    int maxLength = 0;
	for(std::vector<pBayesianUnit>::const_iterator it = vUnits.begin(); it != vUnits.end(); it++)
	{
		BayesianUnit* bayesianUnit = it->get();
        int length = bayesianUnit->getMaxDimension();
		if (length > maxLength)
			maxLength = length;	
    }
	maxLength += 24;
    
    Vec radius(direction.x - center.x, direction.y - center.y);

    unsigned int i, mid, end, add;
    Vec tmpv;
    Position p1, p2;
    if (vUnits.size() % 2 == 0)
    {   
        tmpv = Vec(- radius.y, radius.x); // 90deg rotation
        tmpv.normalize();
        tmpv *= maxLength / 2;
        Vec tmpv2(tmpv);
        tmpv2 *= -1;
        p1 = tmpv.translate(center);
        p2 = tmpv2.translate(center);
        if (!vUnits[0]->unit->getType().isFlyer() && !Broodwar->isWalkable(p1.x()/8, p1.y()/8))
        {
            Position tmp = MapManager::Instance().closestWalkabableSameRegionOrConnected(p1);
            if (tmp != Positions::None)
                p1 = tmp;
        }
        if (!vUnits[1]->unit->getType().isFlyer() && !Broodwar->isWalkable(p2.x()/8, p2.y()/8))
        {
            Position tmp = MapManager::Instance().closestWalkabableSameRegionOrConnected(p2);
            if (tmp != Positions::None)
                p2 = tmp;
        }
        end_positions.push_back(p1);
        end_positions.push_back(p2);
        add = 2;
        mid = (vUnits.size() - 2) / 2;
        end = vUnits.size() - 2;
    }
    else 
    {
        p1 = Position(center.x, center.y);
        p2 = Position(center.x, center.y);
        if (!vUnits[0]->unit->getType().isFlyer() && !Broodwar->isWalkable(p1.x()/8, p1.y()/8))
        {
            Position tmp = MapManager::Instance().closestWalkabableSameRegionOrConnected(p1);
            if (tmp != Positions::None)
                p1 = tmp;
        }
        end_positions.push_back(p1);
        add = 1;
        mid = (vUnits.size() - 1) / 2;
        end = vUnits.size() - 1;
    }

    for (i = 0; i < mid; ++i)
    {
        radius = Vec(direction.x - p1.x(), direction.y - p1.y());
        tmpv = Vec(- radius.y, radius.x);
        tmpv.normalize();
        tmpv *= maxLength;
        p1 = tmpv.translate(p1);
        if (!vUnits[i+add]->unit->getType().isFlyer() && !Broodwar->isWalkable(p1.x()/8, p1.y()/8))
        {
            Position tmp = MapManager::Instance().closestWalkabableSameRegionOrConnected(p1);
            if (tmp != Positions::None)
                p1 = tmp;
        }
        if (p1.isValid())
            end_positions.push_back(p1);
        else
        {
            p1.makeValid();
            end_positions.push_back(p1);
        }
    }
    for (i = mid; i < end; ++i)
    {
        radius = Vec(direction.x - p2.x(), direction.y - p2.y());
        tmpv = Vec(- radius.y, radius.x);
        tmpv.normalize();
        tmpv *= -maxLength;
        p2 = tmpv.translate(p2);
        if (!vUnits[i+add]->unit->getType().isFlyer() && !Broodwar->isWalkable(p2.x()/8, p2.y()/8))
        {
            Position tmp = MapManager::Instance().closestWalkabableSameRegionOrConnected(p2);
            if (tmp != Positions::None)
                p2 = tmp;
        }
        if (p2.isValid())
            end_positions.push_back(p2);
        else
        {
            p2.makeValid();
            end_positions.push_back(p2);
        }
    }

    computeMean();
}
