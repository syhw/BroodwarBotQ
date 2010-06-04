#pragma once
#include "Formation.h"

class SquareFormation: public Formation
{
public:
	SquareFormation(const SquareFormation& f);
	SquareFormation(const Vec& center, const Vec& direction = Vec(0,0));
    SquareFormation(const BWAPI::Position& p, const Vec& direction = Vec(0,0));

	virtual void computeToPositions(const std::vector<pBayesianUnit>& vUnit);
};
