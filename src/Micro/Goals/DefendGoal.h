#pragma once
#include "Micro/Goals/Goal.h"
#include "BWTA.h"
#include <set>

/*** 
 * A helper/wrapper on top of Goal to facilitate defense
 * No subgoals, check completion on Micro
 */
class DefendGoal: public Goal
{
	BWAPI::Position _defPos;
	BWTA::Chokepoint* _choke;
	BWTA::BaseLocation* _base;
	int _eUnits;
	int _nbWorkers;
	inline void bidDefUnits();
	BWAPI::Position _threatMiddle;
public:
	DefendGoal(BWTA::Chokepoint* c, int priority = 80, int firstFrame = 0);
	DefendGoal(BWTA::BaseLocation* b, int priority = 80, int firstFrame = 0);
	DefendGoal(BWAPI::Position p, int priority = 80, int firstFrame = 0);
	virtual void achieve(); // overload
	virtual void check();   // overload

	virtual void onOffer(std::set<BWAPI::Unit*> objects);
	virtual void onRevoke(BWAPI::Unit* u, double bid);
	virtual void onUnitDestroy(BWAPI::Unit* unit);
    virtual std::string getName() const;
	virtual std::string getShortName() const;
	virtual void update();
};
