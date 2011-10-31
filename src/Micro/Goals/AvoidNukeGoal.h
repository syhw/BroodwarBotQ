#pragma once
#include "Micro/Goals/Goal.h"
#include <list>

/*** 
 * A helper/wrapper on top of Goal to facilitate simple attacks (pushs)
 */
class AvoidNukeGoal: public Goal
{
	BWAPI::Position _nukePos;
	BWAPI::Position _safePos;
	std::list<BWAPI::Unit*> _ghostsToKill;
	bool _detectedGhost;
	inline void fleeTheNuke();
public:
	AvoidNukeGoal(BWAPI::Position target);
	void achieve();
};
