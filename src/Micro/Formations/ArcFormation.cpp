#include "ArcFormation.h"

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
	maxLength += 13;
    
    Vec radius(direction.x - center.x, direction.y - center.y);

    unsigned int i, mid, end;
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
        end_positions.push_back(p1);
        end_positions.push_back(p2);
        mid = (vUnits.size() - 2) / 2;
        end = vUnits.size() - 2;
    }
    else 
    {
        p1 = Position(center.x, center.y);
        p2 = Position(center.x, center.y);
        end_positions.push_back(p1);
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
