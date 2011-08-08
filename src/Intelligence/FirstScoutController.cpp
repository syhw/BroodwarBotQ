#include <PrecompiledHeader.h>
#include "Defines.h"
#include "Intelligence/FirstScoutController.h"
#include "Macro/UnitGroupManager.h"
#include "Macro/BWSAL.h"

using namespace BWAPI;

FirstScoutController::FirstScoutController()
: enemyFound(false)
, _notSeenStartLocations(BWTA::getBaseLocations())
{
	_seenStartLocations.insert(BWTA::getNearestBaseLocation(Broodwar->self()->getStartLocation()));
}

FirstScoutController::~FirstScoutController()
{
	TheArbitrator->removeAllBids(this);
}

void FirstScoutController::update()
{
	/********* Infer enemyStartLocation if possible **********/


#ifdef __DEBUG__
	/********* Print enemyStartLocation if known **********/
	if (enemyStartLocation != TilePositions::None)
		Broodwar->drawCircleMap(enemyStartLocation.x()*4, enemyStartLocation.y()*4, 30, Colors::Red, true);
#endif
}


std::string FirstScoutController::getName() const
{
	return "FirstScoutController";
}

void FirstScoutController::harassWorkers()
{
	// TODO
	// hit and run dans les drones aux min
}

void FirstScoutController::check(BWAPI::TilePosition tp)
{
}

void FirstScoutController::onOffer(std::set<Unit*> units)
{
	std::set<Unit*> remainingUnits = units;
	Unit * bestUnit = NULL;
	int dist= 999999999;

	/**** Pick the best units for the jobs ****/
	for (std::list<pGoal>::iterator goal = _awaitingGoals.begin();
		goal != _awaitingGoals.end(); ++goal)
	{
		//find the closest unit for each goal
		bestUnit = NULL;
		for (std::set<Unit*>::iterator unit = remainingUnits.begin();
			unit != remainingUnits.end(); ++unit)
		{
			int tmp = (*goal)->estimateDistance((*unit)->getPosition());
			if (tmp < dist)
			{
				bestUnit = *unit;
				dist = tmp;
			}
		}
		TheArbitrator->accept(this, bestUnit, 100);
		UnitsGroup tmpp;
		tmpp.takeControl(bestUnit);
		remainingUnits.erase(bestUnit);
		tmpp.addGoal((*goal));
		tmpp.switchMode(MODE_SCOUT);
		tmpp.update();
		_unitsGroups.push_back(tmpp);
		distributedGoals.push_back(*goal);
	}
	for (std::list<pGoal>::const_iterator it = distributedGoals.begin();
		it != distributedGoals.end(); ++it)
		_awaitingGoals.remove(*it);

	/**** Decline other units and remove bids ****/
	for (std::set<Unit*>::iterator it = remainingUnits.begin();
		it != remainingUnits.end(); ++it)
	{
		TheArbitrator->decline(this, *it, 0);
		TheArbitrator->removeBid(this, *it);
		_biddedOn.erase(*it);
	}
}


void FirstScoutController::onRevoke(Unit* unit, double bid)
{
	onUnitDestroy(unit);
}

void FirstScoutController::findEnemy()
{
}


void FirstScoutController::onUnitShow(BWAPI::Unit* unit)
{
	if (unit->getPlayer() != Broodwar->enemy())
		return;
	if (!enemyFound && unit->getType() == UnitTypes::Protoss_Nexus 
		|| unit->getType() == UnitTypes::Terran_Command_Center 
		|| unit->getType() == UnitTypes::Zerg_Hatchery
		|| unit->getType() == UnitTypes::Zerg_Lair
		|| unit->getType() == UnitTypes::Zerg_Hive)
	{
		for (std::set<BWTA::BaseLocation*>::const_iterator it = BWTA::getStartLocations().begin(); // fuck BaseLocation that we can't build from a (Tile)Position
			it != BWTA::getStartLocations().end(); ++it)
		{
			if ((*it)->getTilePosition() == unit->getTilePosition())
			{
				enemyFound = true;
				enemyStartLocation = unit->getTilePosition();
				Broodwar->printf("Found enemy here!");
				Broodwar->pingMinimap(unit->getPosition());
				return;
			}
		}
		if (Broodwar->getFrameCount() > 4320) // 4 minutes
		{
			enemyFound = true;
			enemyStartLocation = getNearestStartLocation(unit->getTilePosition());
			Broodwar->printf("Found enemy here!");
			Broodwar->pingMinimap(Position(enemyStartLocation));
		}
	}
}

void FirstScoutController::onUnitDestroy(BWAPI::Unit* unit)
{
	for (std::list<UnitsGroup>::iterator it = _unitsGroups.begin();
		it != _unitsGroups.end(); ++it)
	{
		if (it->removeUnit(unit))
			return;
	}
}

void FirstScoutController::requestScout(double bid)
{
	// Bid on all completed workers + observers/overlords
	std::set<Unit*> usefulUnits = 
		SelectAll()(isWorker,Overlord,Observer)(isCompleted)
		.not(isCarryingMinerals,isCarryingGas,isGatheringGas);
	for (std::set<Unit*>::const_iterator it = usefulUnits.begin();
		it != usefulUnits.end(); ++it)
	{
		if (!_biddedOn.count(*it) && TheArbitrator->getHighestBidder(*it).second < 95)
		{
			TheArbitrator->setBid(this, *it, bid);
			_biddedOn.insert(*it);
		}
	}
}
