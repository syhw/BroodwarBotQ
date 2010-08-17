#pragma once
#include "Formation.h"

class ArcFormation: public Formation
{
public:
	ArcFormation(const ArcFormation& f);
	ArcFormation(const Vec& onCircle, const Vec& circleCenter);
	ArcFormation(const BWAPI::Position& onCircle, const BWAPI::Position& circleCenter);

	virtual void computeToPositions(const std::vector<pBayesianUnit>& vUnits);
};
