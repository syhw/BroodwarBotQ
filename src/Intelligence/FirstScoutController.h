#pragma once
#include "Macro/Arbitrator.h"
#include <BWAPI.h>
#include <BWTA.h>
#include "Intelligence/EUnitsFilter.h"
#include "Micro/UnitsGroup.h"

class GoalManager;

class FirstScoutController : public Arbitrator::Controller<BWAPI::Unit*,double>
{

public:
	FirstScoutController();
	~FirstScoutController();
	std::set<Unit*> _biddedOn;
	std::list<UnitsGroup> _unitsGroups;

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