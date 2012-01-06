#pragma once
#include "Formation.h"

class ArcFormation: public Formation
{
public:
	ArcFormation(const ArcFormation& f);
	ArcFormation(const Vec& onCircle, const Vec& circleCenter, int nonFlyers=0);
	ArcFormation(const BWAPI::Position& onCircle, const BWAPI::Position& circleCenter, int nonFlyers=0);

	virtual void computeToPositions(const std::vector<pBayesianUnit>& vUnits);
};
