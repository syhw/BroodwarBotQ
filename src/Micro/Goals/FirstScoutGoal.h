#pragma once
#include "Micro/Goals/Goal.h"
#include "BWTA.h"
#include <set>

/*** 
 * A helper/wrapper on top of Goal to facilitate simple attacks (pushs)
 */
class FirstScoutGoal: public Goal
{
private:
	bool _foundEnemy;
	BWAPI::TilePosition _nextToVisit;
	BWTA::BaseLocation* _nextBase;
	std::set<BWTA::BaseLocation*> _notSeenStartLocations;
	bool _stealingGas;
	bool _mannerPylon;
	bool _canHarassWorkers;
	std::set<Unit*> _unitsAround;
	inline void goHome();
public:
	FirstScoutGoal(int priority=90);
	void achieve();
	void onOffer(std::set<BWAPI::Unit*> objects);
    virtual std::string getName() const;
};
