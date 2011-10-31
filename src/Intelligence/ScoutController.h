#pragma once
#include "Utils/CSingleton.h"
#include "Macro/Arbitrator.h"
#include <BWAPI.h>
#include <BWTA.h>
#include "Micro/Goals/Goal.h"
#include "Micro/Goals/Subgoal.h"
#include "Micro/Goals/FindEnemyGoal.h"
#include "Intelligence/EUnitsFilter.h"
#include "Micro/Goals/SeeSubgoal.h"
#include "Micro/Goals/FindSubgoal.h"
#include "Micro/UnitsGroup.h"
#include "Micro/Goals/ExploreGoal.h"

class GoalManager;

class ScoutController : public CSingleton<ScoutController>, public Arbitrator::Controller<BWAPI::Unit*,double>
{
	friend class CSingleton<ScoutController>;
	ScoutController();
	~ScoutController();
	std::list<pGoal> _awaitingGoals;
	std::list<UnitsGroup> _unitsGroups;
	bool _requestedScouts;
    std::set<BWTA::BaseLocation*> _seenStartLocations;
	std::set<BWTA::BaseLocation*> _notSeenStartLocations;

public:
	void update();
    virtual std::string getName() const;
	virtual void onOffer(std::set<BWAPI::Unit*> units);
    virtual void onRevoke(BWAPI::Unit* unit, double bid);
	
	void onUnitShow(BWAPI::Unit* unit);
	void onUnitDestroy(BWAPI::Unit* unit);

	void requestScout(double bid);
	void findEnemy();
	void harassWorkers();
	void checkEmptyXP();
	void check(BWAPI::TilePosition tp);
    TilePosition enemyStartLocation;
    std::set<TilePosition> enemyExpandLocations;
    bool enemyFound;
    std::list<TilePosition>::const_iterator lastScoutedLocation;
};