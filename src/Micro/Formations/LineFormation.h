#pragma once
#include "Formation.h"

class LineFormation: public Formation
{
public:
	LineFormation(const LineFormation& f);
	LineFormation(const Vec& center, const Vec& direction = Vec(0,0));
	LineFormation(const BWAPI::Position& p, const Vec& direction = Vec(0,0));

	virtual void computeToPositions(const std::vector<pBayesianUnit>& vUnits);
};
