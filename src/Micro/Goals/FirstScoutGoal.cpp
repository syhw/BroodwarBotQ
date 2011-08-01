#include <PrecompiledHeader.h>
#include "Micro/Goals/FirstScoutGoal.h"
#include "Macro/InformationManager.h"
#include "Macro/BWSAL.h"
#include "Subgoal.h"
#include "Intelligence/Intelligence.h"

using namespace BWAPI;
using namespace std;

inline BWTA::BaseLocation* getNearestBL(const TilePosition& tp, const set<BWTA::BaseLocation*>& s)
{
	BWTA::BaseLocation* ret;
	double min = DBL_MAX;
	for (std::set<BWTA::BaseLocation*>::const_iterator it = s.begin(); 
		it != s.end(); ++it)
	{
		double tmp = BWTA::getGroundDistance((*it)->getTilePosition(), tp);
		if (tmp < min)
		{
			min = tmp;
			ret = *it;
		}
	}
	return ret;
}

FirstScoutGoal::FirstScoutGoal(int priority)
: Goal(priority)
, _foundEnemy(false)
, _nextToVisit(TilePositions::None)
, _nextBase(NULL)
{
	if (BWTA::getStartLocation(Broodwar->self())->getRegion()->getChokepoints().empty()) // if we're on an island
		_status = GS_ACHIEVED;
	_neededUnits.insert(make_pair<UnitType, int>(Broodwar->self()->getRace().getWorker(), 1));
	_notSeenStartLocations = BWTA::getStartLocations();
	_notSeenStartLocations.erase(BWTA::getStartLocation(Broodwar->self()));
	_nextBase = getNearestBL(Broodwar->self()->getStartLocation(), _notSeenStartLocations);
	if (_nextBase != NULL)
		_nextToVisit = _nextBase->getTilePosition();
	_unitsGroup.switchMode(MODE_SCOUT);
}

void FirstScoutGoal::achieve()
{
	if (_status != GS_IN_PROGRESS) // defensive
		return;
	/// They killed our scout before _knowing_ where they are, we will request another scout
	if (!_unitsGroup.size() && TheInformationManager->getEnemyBases().empty())  
		_status = GS_WAIT_PRECONDITION;

	if (!_foundEnemy)
	{
		/// We can see the next waypoint (_nextToVisit)
		if (Broodwar->isVisible(_nextToVisit.x(), _nextToVisit.y()))
		{
			/// If we see a ResourceDepot not belonging to us, it's them
			for each (Unit* u in Broodwar->getUnitsOnTile(_nextToVisit.x(), _nextToVisit.y()))
			{
				if (u->getPlayer() != Broodwar->self() && u->getType().isResourceDepot())
					_foundEnemy = true;
			}
			/// Anyway we don't have to visit here again
			_notSeenStartLocations.erase(_nextBase);
			/// If we haven't found them, go to the next/nearest start location
			if (!_foundEnemy
				&& _notSeenStartLocations.size()) // defensive
			{
				_nextBase = getNearestBL(_nextToVisit, _notSeenStartLocations);
				if (_nextBase != NULL)
					_nextToVisit = _nextBase->getTilePosition();
				_unitsGroup.move(Position(_nextToVisit));
			}
		}
		_unitsGroup.update();
	}

	/// Harass then
	if (_foundEnemy)
	{
		/// Set scoutUnit
		BWAPI::Unit* scoutUnit = NULL;
		if (_unitsGroup.size())
			scoutUnit = (*_unitsGroup.units.begin())->unit;
		else
		{
			/// Our scout got killed but we found them
			_status = GS_ACHIEVED;
			return;
		}
		if (!scoutUnit->exists()) // defensive
			return;

		/// Check if vespene extractor/assimilator/refinery and if not steal (if we have the money), build only one if 2 geysers...
		if (Broodwar->getFrameCount() % 24+Broodwar->getLatencyFrames())
		{
			TilePosition buildGas = TilePositions::None;
			for each (Unit* gas in _nextBase->getGeysers())
			{
				bool canBuildGas = false;
				for each (Unit* u in Broodwar->getUnitsOnTile(gas->getTilePosition().x(), gas->getTilePosition().y()))
				{
					if (u->getPlayer() != Broodwar->self() && u->getType().isRefinery())
						canBuildGas = false;
				}
				if (canBuildGas)
					buildGas = gas->getTilePosition();
			}
			if (buildGas != TilePositions::None && !Intelligence::Instance().enemyRush)
				scoutUnit->build(buildGas, Broodwar->self()->getRace().getRefinery());
		}

		/// Harass otherwise
		// TODO TODO TODO
	}
}

void FirstScoutGoal::onOffer(set<Unit*> objects)
{
	GoalManager* gm = & GoalManager::Instance();
	if ((_status == GS_WAIT_PRECONDITION || _status == GS_IN_PROGRESS)
		&& _unitsGroup.emptyUnits())
	{
		Position choke = (*BWTA::getStartLocation(Broodwar->self())->getRegion()->getChokepoints().begin())->getCenter();
		double minDist = DBL_MAX;
		Unit* acceptedUnit = NULL;
        for each (Unit* u in objects)
		{
			if (gm->getCompletedUnits().find(u) != gm->getCompletedUnits().end()
				&& _neededUnits.find(u->getType()) != _neededUnits.end() // take uniquely needed units
				&& _neededUnits[u->getType()] > 0)
			{
				if (u->getDistance(choke) < minDist)
				{
					acceptedUnit = u;
					minDist = u->getDistance(choke);
				}
			}
		}
		if (acceptedUnit != NULL)
		{
			TheArbitrator->accept(this, acceptedUnit, _priority);
			_neededUnits[acceptedUnit->getType()] -= 1;
			_unitsGroup.dispatchCompleteUnit(gm->getCompletedUnit(acceptedUnit));
			gm->unassignedUnits.erase(acceptedUnit);
		}
	}
	else
	{
		TheArbitrator->decline(this, objects, 0);
		TheArbitrator->removeBid(this, objects);
        for each (Unit* u in objects)
			_biddedOn.erase(u);
	}
}

std::string FirstScoutGoal::getName() const
{
	return "FScout";
}
