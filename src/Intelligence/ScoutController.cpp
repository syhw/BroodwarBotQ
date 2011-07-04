#include <PrecompiledHeader.h>
#include "Defines.h"
#include "Intelligence/ScoutController.h"
#include "Macro/MacroManager.h"
#include "Macro/UnitGroupManager.h"

using namespace BWAPI;

TilePosition getNearestStartLocation(const TilePosition& tp)
{
	TilePosition ret;
	bool first = true;
	double min = 100000000000.0;
	for (std::set<BWTA::BaseLocation*>::const_iterator it = BWTA::getStartLocations().begin(); 
		it != BWTA::getStartLocations().end(); ++it)
	{
		if (first)
		{
			first = false;
			ret = (*it)->getTilePosition();
			continue;
		}
		double tmp = BWTA::getGroundDistance((*it)->getTilePosition(), tp);
		if (tmp < min)
		{
			min = tmp;
			ret = (*it)->getTilePosition();
		}
	}
	return ret;
}

ScoutController::ScoutController()
: enemyFound(false)
, _notSeenStartLocations(BWTA::getBaseLocations())
, _requestedScouts(false)
{
    _seenStartLocations.insert(BWTA::getNearestBaseLocation(Broodwar->self()->getStartLocation()));
}

ScoutController::~ScoutController()
{
    
}

void ScoutController::update()
{
	/********* Infer enemyStartLocation if possible **********/
    if (!enemyFound)
    {
        if (Broodwar->getStartLocations().size() == 2)
        {
            for (std::set<BWTA::BaseLocation*>::const_iterator it = BWTA::getStartLocations().begin(); it != BWTA::getStartLocations().end(); ++it)
            {
                if (!Broodwar->isVisible((*it)->getTilePosition()))
                    enemyStartLocation = (*it)->getTilePosition();
            }
        }
        else if (Broodwar->getStartLocations().size() > 2)
        {
            for (std::set<BWTA::BaseLocation*>::const_iterator it = BWTA::getStartLocations().begin(); it != BWTA::getStartLocations().end(); ++it)
            {
                if (Broodwar->isVisible((*it)->getTilePosition()))
				{
                    _seenStartLocations.insert(*it);
					_notSeenStartLocations.erase(*it);
				}
            }
            if (_seenStartLocations.size() == (BWTA::getStartLocations().size() - 1))
            {
				std::set<BWTA::BaseLocation*>::iterator tmpit = _notSeenStartLocations.begin();
				enemyStartLocation = (*tmpit)->getTilePosition();
            }
        }
    }

#ifdef __DEBUG__
	/********* Print enemyStartLocation if known **********/
    if (enemyStartLocation != TilePositions::None)
        Broodwar->drawCircleMap(enemyStartLocation.x()*4, enemyStartLocation.y()*4, 30, Colors::Red, true);
#endif

	/********* Free units groups that need to be freed and add goals to other *********/
	for(std::list<UnitsGroup>::iterator it = _unitsGroups.begin(); it != _unitsGroups.end(); ++it )
	{
		if (_unitsGroups.empty())
			break;
		if (it->isWaiting())
        {
			//If no new goal available release the ug : 
			if(_awaitingGoals.size() <= 0)
			{
				for (std::vector<pBayesianUnit>::const_iterator u = it->units.begin();
					u != it->units.end(); ++u)
					TheArbitrator->removeBid(this, (*u)->unit);
				_unitsGroups.erase(it);
			}
			else
			{
			//New goal available : assign it to the ug
				it->addGoal(_awaitingGoals.front());
				_awaitingGoals.pop_front();
			}
		}
	}

	/********* Ask units if needed *********/
	if (!_requestedScouts && _awaitingGoals.size() > 0)
    {
		requestScout(95);
	}
}


std::string ScoutController::getName() const
{
	return "ScoutController";
}

void ScoutController::harassWorkers()
{
	// TODO
	// hit and run dans les drones aux min
}

void ScoutController::check(BWAPI::TilePosition tp)
{
	_awaitingGoals.push_back(pGoal(new ExploreGoal(tp)));
}

void ScoutController::onOffer(std::set<Unit*> units)
{
	std::set<Unit*> remainingUnits = units;
	Unit * bestUnit = NULL;
	int dist= 999999999;
	std::list<pGoal> distributedGoals;

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
	}
	_requestedScouts = false;
}


void ScoutController::onRevoke(Unit* unit, double bid)
{
	onUnitDestroy(unit);
}

void ScoutController::findEnemy()
{
	_awaitingGoals.push_front(pGoal(new FindEnemyGoal()));
}


void ScoutController::onUnitShow(BWAPI::Unit* unit)
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

void ScoutController::onUnitDestroy(BWAPI::Unit* unit)
{
	for (std::list<UnitsGroup>::iterator it = _unitsGroups.begin();
		it != _unitsGroups.end(); ++it)
	{
	    if (it->removeUnit(unit))
			return;
	}
}

void ScoutController::requestScout(double bid)
{
  // Bid on all completed workers + observers/overlords
  std::set<Unit*> usefulUnits = 
	  SelectAll()(isWorker,Overlord,Observer)(isCompleted)
	  .not(isCarryingMinerals,isCarryingGas,isGatheringGas);
  TheArbitrator->setBid(this, usefulUnits, bid);
  _requestedScouts = true;
}
