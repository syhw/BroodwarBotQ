#pragma once
#include <CSingleton.h>
#include <Arbitrator.h>
#include <BWAPI.h>
#include <Regions.h>
#include <BWTA.h>
#include "Goal.h"
#include "Subgoal.h"
#include "FindEnemyGoal.h"
#include "EUnitsFilter.h"
#include "SeeSubgoal.h"
#include "FindSubgoal.h"
#include "WarManager.h"
#include "BaseObject.h"
#include "ExploreGoal.h"

class GoalManager;

class ScoutManager :  public CSingleton<ScoutManager>, public Arbitrator::Controller<BWAPI::Unit*,double>
{
	friend class CSingleton<ScoutManager>;
	ScoutManager();
	~ScoutManager();
	WarManager * warManager;
	Arbitrator::Arbitrator<BWAPI::Unit*,double>* arbitrator;
	std::list<pGoal> awaitingGoals;
	bool exploringEnemy;
	std::list<UnitsGroup *> myUnitsGroups;

public:
	void setDependencies();
	virtual void update();
	void onOffer(std::set<BWAPI::Unit*> units);
    virtual void onRevoke(BWAPI::Unit* unit, double bid);
	virtual std::string getName() const;
	
	void onUnitCreate(BWAPI::Unit* unit);
	void onUnitShow(BWAPI::Unit* unit);
	void onUnitDestroy(BWAPI::Unit* unit);

	void findEnemy();
	
	//TODO
	void scoutAllEnemies();
	void counterWall();
	void counterBuild();
	void harassWorkers();
	void checkEmptyXP();
    TilePosition enemyStartLocation;
    std::set<TilePosition> enemyExpandLocations;
    bool enemyFound;
    std::list<TilePosition>::const_iterator lastScoutedLocation;
};