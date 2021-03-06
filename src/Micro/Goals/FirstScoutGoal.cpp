#include <PrecompiledHeader.h>
#include "Micro/Goals/FirstScoutGoal.h"
#include "Macro/InformationManager.h"
#include "Macro/BWSAL.h"
#include <BWTA.h>
#include "Subgoal.h"
#include "Intelligence/Intelligence.h"
#include "Regions/MapManager.h"
#include "Micro/Micro.h"
#include "Defines.h"

using namespace BWAPI;
using namespace std;

inline BWTA::BaseLocation* getNearestBL(const TilePosition& tp, const set<BWTA::BaseLocation*>& s)
{
	BWTA::BaseLocation* ret = *(s.begin());
	double min = DBL_MAX;
	for (std::set<BWTA::BaseLocation*>::const_iterator it = s.begin(); 
		it != s.end(); ++it)
	{
		double tmp = MapManager::Instance().distRegions((*it)->getRegion(), BWTA::getRegion(tp));
		if (tmp > 0 && tmp < min)
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
, _gatheredIntel(false)
, _stealingGas(true)
, _mannerPylon(false)
, _canHarassWorkers(true)
, _arrivePosition(Positions::None)
, _mineral(NULL)
{
	BWTA::BaseLocation* home = BWTA::getStartLocation(Broodwar->self());
	if (!home->getMinerals().empty())
		_mineral = *(home->getMinerals().begin());
	if (home->getRegion()->getChokepoints().empty()) // if we're on an island
		_status = GS_ACHIEVED;
	_neededUnits.insert(make_pair<UnitType, int>(Broodwar->self()->getRace().getWorker(), 1));
	_notSeenStartLocations = BWTA::getStartLocations();
	_notSeenStartLocations.erase(BWTA::getStartLocation(Broodwar->self()));
	_nextBase = getNearestBL(Broodwar->self()->getStartLocation(), _notSeenStartLocations);
	if (_nextBase != NULL)
		_nextToVisit = _nextBase->getTilePosition();
	_unitsGroup.switchMode(MODE_SCOUT); // will use the scouting unit as leading units and update its path with threat aware pathfinding
}

void FirstScoutGoal::achieve()
{
	if (_status != GS_IN_PROGRESS) // defensive
		return;
	/// If we have not unit, either we know where they are, or we need to send another scout
	if (!_unitsGroup.size())
	{
		if (_notSeenStartLocations.size() > 1)
		{
			/// They killed our scout before _knowing_ where they are, we will request another scout
			_status = GS_WAIT_PRECONDITION;
			return;
		}
		else // either we found them or at least we know where they are
		{
			_status = GS_ACHIEVED;
			return;
		}
	}

	/// Set scoutUnit
	BWAPI::Unit* scoutUnit = NULL;
	if (_unitsGroup.size())
		scoutUnit = (*_unitsGroup.units.begin())->unit;
	if (!scoutUnit->exists()) // defensive
		return;

	/// TODO change the moves for minerals clicks (mineral walking)

	if (!_foundEnemy)
	{
		/// We can see the next waypoint (_nextToVisit)
		if (Broodwar->isVisible(_nextToVisit.x(), _nextToVisit.y()))
		{
			/// If we see a ResourceDepot not belonging to us, it's them
			for each (Unit* u in Broodwar->getUnitsOnTile(_nextToVisit.x(), _nextToVisit.y()))
			{
				if (u->getPlayer() != Broodwar->self() && u->getType().isResourceDepot())
				{
					_foundEnemy = true;
					break;
				}
			}
			/// Anyway we don't have to visit here again
			_notSeenStartLocations.erase(_nextBase);
			/// If we haven't found them, go to the next/nearest start location
			if (!_foundEnemy
				&& _notSeenStartLocations.size()) // defensive
			{
				if (_notSeenStartLocations.size() == 1) // infer where the enemy is
					TheInformationManager->addEnemyBase(*_notSeenStartLocations.begin());
				_nextBase = getNearestBL(_nextToVisit, _notSeenStartLocations);
				if (_nextBase != NULL)
					_nextToVisit = _nextBase->getTilePosition();
				_unitsGroup.move(Position(_nextToVisit));
				_arrivePosition = _unitsGroup.getCenter();
			}
		}
		else
		{
			/*
			if (BWTA::getStartLocation(Broodwar->self())->getRegion()
				== BWTA::getRegion(TilePosition(_unitsGroup.center))
				&& _unitsGroup.getDistance(Micro::Instance().frontChoke->getCenter()) > 51) // can't base ourselved on the choke's center as the unit will stop while still in our region erratically
			{
				// still at home, trick to avoid going into sucking destructible backdoors/chokes
				_unitsGroup.move(Micro::Instance().frontChoke->getCenter());
			}
			else
				_unitsGroup.move(Position(_nextToVisit));
			if (_unitsGroup.groupMode != MODE_SCOUT)
				_unitsGroup.switchMode(MODE_SCOUT);
			_unitsGroup.update();
			_arrivePosition = _unitsGroup.getCenter();
			*/
			scoutUnit->move(Position(_nextToVisit));
			_arrivePosition = scoutUnit->getPosition();
			return;
		}
	}
	else /// Found enemy, harass then
	{
		if (Intelligence::Instance().enemyRush) // we are being rushed
			_stealingGas = false;
		
		if (scoutUnit->getDistance(Position(Broodwar->self()->getStartLocation())) < 20*TILE_SIZE)
		{
			_status = GS_ACHIEVED;
			return;
		}

#ifdef __DO_NOT_HARASS_SCOUT__
		goHome(scoutUnit);
#else
		if (!_gatheredIntel)
		{
			_unitsAround = Broodwar->getUnitsInRadius(scoutUnit->getPosition(), 10*TILE_SIZE);
			Position c(Positions::None);
			for each (Unit* u in _unitsAround)
			{
				if (u->getType().isResourceDepot())
				{
					c = u->getPosition();
					break;
				}
			}
			BWTA::BaseLocation* b = NULL;
			if (!c.isValid() || c == Positions::None)
				b = BWTA::getNearestBaseLocation(scoutUnit->getPosition());
			else
				b = BWTA::getNearestBaseLocation(c);
			if (b == NULL)
			{ 
				// hu?
				goHome(scoutUnit);
				return;
			}
			Vec dir(c.x() - _arrivePosition.x(), c.y() - _arrivePosition.y());
			dir.normalize();
			dir *= 5*TILE_SIZE;
			Position pos(dir.translate(c));
			pos.makeValid();
			if ((Broodwar->enemy()->getRace() == Races::Zerg 
				&& EUnitsFilter::Instance().getNumbersType(UnitTypes::Zerg_Spawning_Pool))
				|| (Broodwar->enemy()->getRace() == Races::Terran
				&& EUnitsFilter::Instance().getNumbersType(UnitTypes::Terran_Barracks))
				|| (Broodwar->enemy()->getRace() == Races::Protoss
				&& EUnitsFilter::Instance().getNumbersType(UnitTypes::Protoss_Gateway))
				|| scoutUnit->getDistance(pos) < 3*TILE_SIZE
				|| scoutUnit->getHitPoints() < 20) // they cut through shield
				_gatheredIntel = true;
			Vec side(0,0);
			for each (Unit* u in b->getMinerals())
				side += Vec(c.x() - u->getPosition().x(), c.y() - u->getPosition().y());
			side.normalize();
			side *= 5*TILE_SIZE;
			Position mid(side.translate(c));
			mid.makeValid();
			if (scoutUnit->getDistance(pos) > 11*TILE_SIZE)
				_unitsGroup.move(mid);
			else
				_unitsGroup.move(pos);
			if (_unitsGroup.groupMode != MODE_SCOUT)
				_unitsGroup.switchMode(MODE_SCOUT);
			_unitsGroup.update();
		}

		if (_stealingGas)
		{
			/// Check if vespene extractor/assimilator/refinery and if not steal (if we have the money), build only one if 2 geysers...
			if (scoutUnit->isUnderAttack() || scoutUnit->getShields() < 2)
			{
				micro(scoutUnit);
				return;
			}
			TilePosition buildGas = TilePositions::None;
			for each (Unit* gas in _nextBase->getGeysers())
			{
				if (!Broodwar->isVisible(gas->getTilePosition()) && scoutUnit->getDistance(Position(_nextToVisit)) > 4*TILE_SIZE)
				{
					scoutUnit->move(Position(_nextToVisit));
					return;
				}
				for each (Unit* u in Broodwar->getUnitsOnTile(gas->getTilePosition().x(), gas->getTilePosition().y()))
				{
					if (u->getType().isRefinery())
						_stealingGas = false;
				}
				if (_stealingGas)
					buildGas = gas->getTilePosition();
			}
			if (buildGas != TilePositions::None)
			{
				scoutUnit->build(buildGas, Broodwar->self()->getRace().getRefinery());
				return;
			}
			else
				_stealingGas = false;
			return;
		}

		/// Check if we can manner pylon
		// TODO manner pylon
		// !Intelligence::Instance().enemyRush

		/// Harass units then
		if (!_mannerPylon)
		{
			if (scoutUnit->getHitPoints() < 11)
			{
				_canHarassWorkers = false;
				goHome(scoutUnit);
				return;
			}
			if (!(Broodwar->getFrameCount() % (1 + Broodwar->getLatencyFrames())))
			{
				micro(scoutUnit);
				return;
			}
		}

		if (!_canHarassWorkers)
			goHome(scoutUnit);
#endif
	}
}

void FirstScoutGoal::micro(Unit* scoutUnit)
{
	// because we're too hype to update units from filter (UnitsGroup)
	_unitsAround = Broodwar->getUnitsInRadius(scoutUnit->getPosition(), 192);
	set<Unit*> attackingMe;
	Unit* closestWorker = NULL;
	double closestDist = DBL_MAX;
	for each (Unit* u in _unitsAround)
	{
		if (u->getType().groundWeapon().maxRange() > 51 // > sqrt(2*TILE_SIZE^2) && < 2*TILE_SIZE
			|| (u->getType() == UnitTypes::Zerg_Zergling && u->getDistance(scoutUnit) < 3*TILE_SIZE))
			_canHarassWorkers = false;
		if (u->getTarget() == scoutUnit && u->getType().canAttack())
			attackingMe.insert(u);
		if (u->getPlayer() == Broodwar->enemy()
			&& u->getType().isWorker()
			&& u->getDistance(scoutUnit) < closestDist)
		{
			closestDist = u->getDistance(scoutUnit);
			closestWorker = u;
		}
	}
	if (attackingMe.empty())
	{
		if (closestWorker == NULL)
			scoutUnit->attack(Position(_nextToVisit));
		else
			scoutUnit->attack(closestWorker);
		return;
	}
	else
	{
		scoutUnit->rightClick(_mineral); //direction.translate(scoutUnit->getPosition()));
		return;
	}
}

void FirstScoutGoal::goHome(Unit* scoutUnit)
{
	if (_unitsGroup.groupMode != MODE_SCOUT) // defensive
		_unitsGroup.switchMode(MODE_SCOUT);
	Position middle(Broodwar->mapWidth()*TILE_SIZE/2, Broodwar->mapHeight()*TILE_SIZE/2);
	scoutUnit->rightClick(_mineral);
	_status = GS_ACHIEVED;
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
			pBayesianUnit tmp = gm->getCompletedUnit(acceptedUnit);
			if (tmp) // we only accept complete units but defensive
				_unitsGroup.dispatchCompleteUnit(tmp);
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
