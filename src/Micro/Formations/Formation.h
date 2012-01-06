#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <math.h>
#include <windows.h>
#include <Utils/Vec.h>
#include <Micro/Units/BayesianUnit.h>

#ifndef _SMART_POINTER_FORMATION_INCLUDED
#define _SMART_POINTER_FORMATION_INCLUDED 1
#include <boost/shared_ptr.hpp>
class Formation;
typedef boost::shared_ptr<Formation> pFormation;
#endif

class Formation 
{
public:
	Vec center;
	Vec direction;
    Vec mean;
	std::vector<BWAPI::Position> end_positions;
	int space; // space between units. 0 means units are only sperated by their sizes.
	int non_flyers;

	Formation(const Formation& f);
	Formation(const Vec& center, int nonFlyers=0, const Vec& direction = Vec(0,0));
	Formation(const BWAPI::Position& p, int nonFlyers=0, const Vec& direction = Vec(0,0));
	~Formation();

    void computeMean();
	void checkCenterReachability();
    // TODO should be called on onUnitDestroy for us
	virtual void computeToPositions(const std::vector<pBayesianUnit>& vUnit);
};
