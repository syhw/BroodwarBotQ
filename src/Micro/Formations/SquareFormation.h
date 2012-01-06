#pragma once
#include "Formation.h"

class SquareFormation: public Formation
{
public:
	SquareFormation(const SquareFormation& f);
	SquareFormation(const Vec& center, int nonFlyers=0, const Vec& direction = Vec(0,0));
    SquareFormation(const BWAPI::Position& p, int nonFlyers=0, const Vec& direction = Vec(0,0));

	virtual void computeToPositions(const std::vector<pBayesianUnit>& vUnits);
	virtual void computeToPositions(const std::vector<pBayesianUnit>& vUnits, unsigned int unwantedSlots);
};
