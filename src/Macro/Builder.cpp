#include <PrecompiledHeader.h>
#include "Macro/Macro.h"
#include "Macro/Builder.h"
#include "BWSAL.h"
#include "Macro/ReservedMap.h"
#include "Macro/UnitGroupManager.h"
#include "Defines.h"

using namespace BWAPI;
using namespace std;

#define __MIN_HP_CANCEL_BUILDING_IN_CONSTRUCTION__ 50
#define __MIN_FRAMES_TO_START_CONSTRUCTION__ 10*24

SimCityBuildingPlacer* Task::buildingPlacer = NULL;


Task::Task(BWAPI::Unit* w, BWAPI::TilePosition tp, BWAPI::UnitType ut, int lo)
: worker(w)
, tilePosition(tp)
, type(ut)
, lastOrder(lo)
, tries(0)
, positioned(false)
, powered(false)
, reserved(false)
, finished(false)
{
	if (type == UnitTypes::Protoss_Pylon || type == UnitTypes::Protoss_Assimilator || type == UnitTypes::Protoss_Nexus)
		powered = true;
}

Task::~Task()
{
	if (reserved)
	{
		Macro::Instance().reservedMinerals -= type.mineralPrice();
		Macro::Instance().reservedGas -= type.gasPrice();
	}
	TheArbitrator->removeController(this);
}

int Task::framesToCompleteRequirements(UnitType type)
{
	int ret = 0;
	map<UnitType, int> rqU = type.requiredUnits();
	for (map<UnitType, int>::const_iterator it = rqU.begin();
		it != rqU.end(); ++it)
	{
		if (Broodwar->self()->completedUnitCount(it->first) == 0) //< it->second)
		{
			/// We don't have the requirements
			/// we will have to delay this construction
			int delay = 0;
			if (it->first.isBuilding()
				&& Broodwar->self()->incompleteUnitCount(it->first) == 0)//< it->second)
			{
				/// We are not building the requirement, get it into the production line
				if (!TheBuilder->willBuild(it->first))
					TheBuilder->build(it->first); // could recurse on the last addition to the Task list but it's evil
				delay += it->first.buildTime() + __MIN_FRAMES_TO_START_CONSTRUCTION__; // not perfect
			}
			else
			{
				/// We are building this requirement, how much time before completion?
				for each (Unit* u in TheBuilder->getInConstruction())
				{
					if (u->getType() == it->first) // not really accurate (we take the first one and not the min), but not really hapening that 2 requirements are in construction at the same time
					{
						delay += u->getRemainingBuildTime();
						break;
					}
				}
			}
			ret = max(ret, delay);
		}
	}
	return ret;
}

bool Task::requirements()
{
	/// side effect function which builds the requirements
	int delay = Task::framesToCompleteRequirements(type); // launches the requirements
	if (delay)
	{
		lastOrder = max(lastOrder, Broodwar->getFrameCount() + delay);
		return false;
	}
	else
		return true;
}

void Task::positionIt()
{
	if (tilePosition == TilePositions::None || tilePosition == TilePositions::Invalid || tilePosition == TilePositions::Unknown
		|| !tilePosition.isValid() || 
		(type != UnitTypes::Protoss_Assimilator && !Broodwar->isBuildable(tilePosition.x(), tilePosition.y(), true)))
		tilePosition = buildingPlacer->getTilePosition(type);
	if (tilePosition == TilePositions::None || tilePosition == TilePositions::Invalid || tilePosition == TilePositions::Unknown
		|| !tilePosition.isValid())
	{
		finished = true;
#ifdef __MACRO_DEBUG__
		Broodwar->printf("\x08 !!! CANCELED Building %s at (%d,%d) !!! NO POSITION", type.getName().c_str(), tilePosition.x(), tilePosition.y());
#endif
	}
	else
		positioned = true;
}

void Task::onOffer(set<Unit*> units)
{
	if (worker != NULL && worker->exists()) // defensive prog
	{
		// not sure about that or re-taking a (new) working
		TheArbitrator->decline(this, units, 0);
		TheArbitrator->removeBid(this, units);
		return;
	}

	if (tilePosition.isValid())
	{
		double dist = DBL_MAX;
		for (set<Unit*>::const_iterator it = units.begin();
			it != units.end(); ++it)
		{
			double tmpDist = tilePosition.getDistance((*it)->getTilePosition());
			if (tmpDist < dist)
			{
				dist = tmpDist;
				worker = *it;
			}
		}
	}
	else
	{
		if (!units.empty())
			worker = (*units.begin());
	}

	if (worker != NULL)
	{
		units.erase(worker);
		TheArbitrator->accept(this, worker, 100);
		for (set<Unit*>::const_iterator it = units.begin();
			it != units.end(); ++it)
		{
			TheArbitrator->decline(this, *it, 0);
			TheArbitrator->removeBid(this, *it);
		}
	}
}

void Task::onRevoke(BWAPI::Unit* unit, double bid)
{
	TheArbitrator->removeAllBids(this);
	if (finished)
		return;
	if (unit == worker)
		askWorker();
}

void Task::askWorker()
{
	TheArbitrator->removeAllBids(this);
	if (finished)
		return;
	std::set<Unit*> usefulUnits = 
		SelectAll()(isWorker)(isCompleted)
		.not(isCarryingGas,isGatheringGas);
	TheArbitrator->setBid(this, usefulUnits, 60); // TODO change 60
}

void Task::powerIt()
{
	if (!Broodwar->hasPower(tilePosition))
	{
		TilePosition tmpTilePosition(tilePosition.x()+type.tileWidth()/2, tilePosition.y()+type.tileHeight()/2);
		powered = false;
		/// Look for a pylon which will be built and will cover
		bool foundCovering = false;
		for each (Unit* pic in TheBuilder->getInConstruction(UnitTypes::Protoss_Pylon))
		{
			if (tmpTilePosition.getDistance(TilePosition(pic->getTilePosition().x()+1, pic->getTilePosition().y()+1)) < __PYLON_COVERAGE_TILES__*TILE_SIZE)
			{
				foundCovering = true;
				break;
			}
		}
		for each (TilePosition tp in TheBuilder->getFutureTaskPos(UnitTypes::Protoss_Pylon, this))
		{
			if (tp == TilePositions::None || tp == TilePositions::Invalid || tp == TilePositions::Unknown)
				return;
			if (tmpTilePosition.getDistance(TilePosition(tp.x()+1, tp.y()+1)) < __PYLON_COVERAGE_TILES__*TILE_SIZE)
			{
				foundCovering = true;
				break;
			}
		}
		if (!foundCovering)
		{
			/// Seek a covering pylon position
			TilePosition pylonTP = buildingPlacer->getPylonTilePositionCovering(tilePosition);
			if (pylonTP != TilePositions::None)
			{
				/// Ask for a powering pylon
				TheBuilder->build(UnitTypes::Protoss_Pylon, pylonTP);
				lastOrder = Broodwar->getFrameCount() 
					+ UnitTypes::Protoss_Pylon.buildTime()
					+ __MIN_FRAMES_TO_START_CONSTRUCTION__
					+ (int)(100.0 /TheResourceRates->getGatherRate().getMinerals()); // delay our construction, assume we have no money
			}
			else
			{
				finished = true; // abandon construction
#ifdef __MACRO_DEBUG__
				Broodwar->printf("\x08 !!! CANCELED Building %s at (%d,%d) !!! NO POWER", type.getName().c_str(), tilePosition.x(), tilePosition.y());
#endif
			}
		}
	}
	else
		powered = true;
}

void Task::buildIt()
{
	if (tries > __MAX_TRIES_BUILD_SOMETHING__)
	{
		finished = true;
#ifdef __MACRO_DEBUG__
		Broodwar->printf("\x08 !!! CANCELED Building %s at (%d,%d) !!! TOO MUCH TRIES", type.getName().c_str(), tilePosition.x(), tilePosition.y());
#endif
		return;
	}
	if (!worker || !worker->exists() || worker->getPlayer() != Broodwar->self())
	{
		/// Just in case the worker got killed / captured
		worker = NULL;
		askWorker();
		return;
	}
	++tries;
	/*if (tilePosition == TilePositions::None || tilePosition == TilePositions::Invalid
		|| tilePosition == TilePositions::Invalid || tilePosition == TilePosition(0,0))
	{
		buildingPlacer->getTilePosition(type);
		return;
	}*/
	if (Broodwar->getFrameCount() > lastOrder + 17 + Broodwar->getLatencyFrames())
	{
		if (finished)
			return;

		/// Move closer to the construction site
		if (worker->getDistance(Position(tilePosition)) > 6 * TILE_SIZE) // 6 > sqrt(biggest^2+biggest^2) (biggest buildings as Protoss are nexus/gates)
		{
			if (Macro::Instance().expands == 1 && type == UnitTypes::Protoss_Nexus && worker->getDistance(Position(tilePosition)) > 9*TILE_SIZE)
			{
				BWTA::BaseLocation* b = BWTA::getNearestBaseLocation(tilePosition);
				if (b != NULL && !b->getMinerals().empty())
					worker->rightClick(*(b->getMinerals().begin()));
				else
					worker->move(Position(tilePosition));
			}
			else
				worker->move(Position(tilePosition));
			//worker->move(Position(tilePosition).x() - 1, Position(tilePosition).y() - 1);
		}
		/// Try and build it if we can
		else if (!worker->build(tilePosition, type))
		{ /// TODO time_to_rebuild *= 2
#ifdef __MACRO_DEBUG__
			Broodwar->printf("ERROR: Can't build %s at (%d,%d)", type.getName().c_str(), tilePosition.x(), tilePosition.y());
#endif
		}
		lastOrder = Broodwar->getFrameCount();
	}
}

TilePosition Task::getTilePosition() const
{
	return tilePosition;
}

string Task::getName() const
{
	char tmp[63];
	if (tilePosition != TilePositions::None)
		sprintf_s(tmp, "%s in (%d,%d)", type.getName().c_str(), tilePosition.x(), tilePosition.y());
	else
		sprintf_s(tmp, "%s", type.getName().c_str());
	return string(tmp);
}

string Task::getShortName() const
{
	return getName();
}

void Task::check()
{	
	if (Broodwar->getFrameCount() <= lastOrder) // delay hack
		return;
	lastOrder = max(lastOrder, Task::framesToCompleteRequirements(type)); 
	//if (tilePosition == TilePositions::None)
	//	buildingPlacer->getTilePosition(type);
	/// Check if we have finished, or if there are blocking units we can move,
	/// or if the build TilePosition is really blocked
	for (set<Unit*>::const_iterator it = Broodwar->getUnitsOnTile(tilePosition.x(), tilePosition.y()).begin();
		it != Broodwar->getUnitsOnTile(tilePosition.x(), tilePosition.y()).end(); ++it)
	{
		if (*it == worker)
			continue;
		UnitType tmp = (*it)->getType();
		if ((*it)->getPlayer() != Broodwar->self() && (*it)->getPlayer() != Broodwar->neutral()) // then we can't move this unit
		{
			if (tmp == UnitTypes::Protoss_Assimilator || tmp == UnitTypes::Zerg_Extractor || tmp == UnitTypes::Terran_Refinery)
			{
				/// Retry in a few seconds because the fucker stole our gas
				TheArbitrator->removeBid(this, worker);
				TheArbitrator->removeAllBids(this);
				worker = NULL;
				lastOrder = Broodwar->getFrameCount() + 14*24; // retry every 14 seconds
				return;
			}
			else
				tilePosition = buildingPlacer->getTilePosition(type);
			break;
		}
		if (tmp == type)
		{
			TheArbitrator->removeBid(this, worker);
			TheArbitrator->removeAllBids(this);
			finished = true;
			return;
		}
		else if (tmp.canMove())
		{
#ifdef __MACRO_DEBUG__
			Broodwar->printf("A unit is blocking the construction in %d, %d", tilePosition.x(), tilePosition.y());
#endif
			(*it)->move(Position(Broodwar->self()->getStartLocation())); // try and move the unit, TODO will block if the unit doesn't move
		}
		else if (!(tmp == UnitTypes::Resource_Vespene_Geyser && type == Broodwar->self()->getRace().getRefinery()))
		{
			tilePosition = buildingPlacer->getTilePosition(type); // really blocked (can't move)
			break;
		}
	}
}

void Task::update()
{
	if (finished)
		return;
	if (Broodwar->getFrameCount() <= lastOrder) // delay hack
		return;
	if (!requirements())
		return;
	if (!positioned)
	{
		positionIt();
		return;
	}
	if (!powered)
	{
		powerIt();
		return;
	}
	if (!reserved)
	{
		Macro::Instance().reservedMinerals += type.mineralPrice();
		Macro::Instance().reservedGas += type.gasPrice();
		reserved = true;
	}

	// TODO complete distance instead of magic number "6" tiles.
	double timeToGetToSite = ((double)(6*TILE_SIZE) / UnitTypes::Protoss_Probe.topSpeed());
	if (Broodwar->self()->minerals() < type.mineralPrice() - (TheResourceRates->getGatherRate().getMinerals() * timeToGetToSite)
		&& Broodwar->self()->gas() < type.gasPrice() - (TheResourceRates->getGatherRate().getGas() * timeToGetToSite))
		return;
	else if (worker == NULL || !worker->exists())
		askWorker();
	else
		buildIt();
}

const UnitType& Task::getType() const
{
	return type;
}

int Task::getLastOrder() const
{
	return lastOrder;
}

bool Task::hasReserved()
{
	return reserved;
}

bool Task::isFinished()
{
	return finished;
}

Builder* TheBuilder = NULL;

Builder* Builder::create()
{
	TheReservedMap = ReservedMap::create();
	if (TheBuilder)
		return TheBuilder;
	TheBuilder = new Builder();
	return TheBuilder;
}

void Builder::destroy()
{
	ReservedMap::destroy();
	if (TheBuilder)
		delete TheBuilder;
}

Builder::Builder()
{
	TheBuilder = this;
	Task::buildingPlacer = & this->buildingPlacer;
}

Builder::~Builder()
{
	tasks.clear();
	TheBuilder = NULL;
}

// Use with care outside of here
void Builder::addTask(const UnitType& t, const TilePosition& seedPosition, bool quick, int lastOrder)
{
	pTask tmp(new Task(NULL, seedPosition, t, lastOrder));
	if (quick)
		tasks.push_front(tmp);
	else
		tasks.push_back(tmp);
}

void Builder::build(const BWAPI::UnitType& t, const BWAPI::TilePosition& seedPosition, bool quick)
{
	if (t == Broodwar->self()->getRace().getCenter())
		TheBasesManager->expand();
	else
		addTask(t, seedPosition, quick);
}

/// Attempts to build 2 pylons + 2 cannons around the minerals
void Builder::buildCannonsMinerals(BWTA::BaseLocation* b)
{
	if (b == NULL)
		return;
	Task::buildingPlacer->makeCannonsMinerals(b);
	// 2 pylons
	TilePosition tp1 = Task::buildingPlacer->getTilePosition(UnitTypes::Protoss_Pylon, b->getTilePosition());
	if (tp1 == TilePositions::None)
		return;
	addTask(UnitTypes::Protoss_Pylon, tp1, true);
	TilePosition tp2 = Task::buildingPlacer->getTilePosition(UnitTypes::Protoss_Pylon, b->getTilePosition());
	if (tp2 == TilePositions::None)
		return;
	addTask(UnitTypes::Protoss_Pylon, tp2, true);
	// 2 cannons
	TilePosition tp3 = Task::buildingPlacer->getTilePosition(UnitTypes::Protoss_Photon_Cannon, tp1);
	if (tp3 == TilePositions::None)
		return;
	addTask(UnitTypes::Protoss_Photon_Cannon, tp3, false, Broodwar->getFrameCount() + UnitTypes::Protoss_Pylon.buildTime() + __MIN_FRAMES_TO_START_CONSTRUCTION__);
	tp3 = Task::buildingPlacer->getTilePosition(UnitTypes::Protoss_Photon_Cannon, tp2);
	if (tp3 == TilePositions::None)
		return;
	addTask(UnitTypes::Protoss_Photon_Cannon, tp3, false, Broodwar->getFrameCount() + UnitTypes::Protoss_Pylon.buildTime() + __MIN_FRAMES_TO_START_CONSTRUCTION__);
}

//void Builder::buildCannonsNexus(BWTA::BaseLocation* b) TODO

int Builder::numberInFutureTasks(const UnitType& t)
{
	int number = 0;
	for each (pTask task in tasks)
		if (task->getType() == t)
			++number;
	return number;
}

int Builder::numberInConstruction(const UnitType& t)
{
	return Broodwar->self()->incompleteUnitCount(t);
	/*
	int number = 0;
	for each (Unit* u in inConstruction)
		if (u->getType() == t)
			++number;
	return number;*/
}

/***
 * Counts the number of buildings of type _t_ that we will be built
 * (currently in the tasks pipeline or in construction)
 */
int Builder::willBuild(const UnitType& t)
{
	return numberInFutureTasks(t) + numberInConstruction(t);
}

const list<Unit*>& Builder::getInConstruction()
{
	return inConstruction;
}

list<Unit*> Builder::getInConstruction(const UnitType& t)
{
	list<Unit*> r;
	for each (Unit* u in inConstruction)
	{
		if (u->getType() == t)
			r.push_back(u);
	}
	return r;
}

list<TilePosition> Builder::getFutureTaskPos(const UnitType& t, Task* before)
{
	list<TilePosition> r;
	for each (pTask task in tasks)
	{
		if (task.get() == before)
			return r;
		if (task->getType() == t)
			r.push_back(task->getTilePosition());
	}
	return r;
}

/*** 
 * Computes the number of additional supply that we can hope to have
 * for _frames_ later.
 * Should perhaps be done with events (maintaining an "additionalSupply" int)
 */
int Builder::additionalSupplyNextFrames(int frames)
{
	int supply = 0;
	if (frames > Broodwar->self()->getRace().getSupplyProvider().buildTime() + __MIN_FRAMES_TO_START_CONSTRUCTION__)
	{
		for each (pTask task in tasks)
		{
			if (task->getType().buildTime() < frames)
				supply += task->getType().supplyProvided();
		}
	}
	for each (Unit* u in inConstruction)
	{
	    if (u->getRemainingBuildTime() > frames)
			continue;
		supply += u->getType().supplyProvided();
	}
#ifdef __MACRO_DEBUG__
	Broodwar->drawTextScreen(190, 56, "\x11 incS: %d", supply);
#endif
	return supply;
}

/***
* It should be the last update()/onFrame() call (to move "blocking construction" units)
*/
void Builder::update()
{
	/// Perform tasks
	double meanTimeToGetToSite = ((double)(6*TILE_SIZE) / UnitTypes::Protoss_Probe.topSpeed()); // TODO work on this
	int additionalMinerals = (int)(meanTimeToGetToSite * TheResourceRates->getGatherRate().getMinerals()) + 1;
	int additionalGas = (int)(meanTimeToGetToSite * TheResourceRates->getGatherRate().getGas()) + 1;
	int minerals = Broodwar->self()->minerals() + additionalMinerals;
	int gas = Broodwar->self()->gas() + additionalGas;

	for (list<pTask>::iterator it = tasks.begin();
		it != tasks.end();)
	{
		list<pTask>::iterator tmp = it++;
		if ((*tmp)->isFinished())
			tasks.erase(tmp);
		else
		{
			(*tmp)->check();
			/// update as much construction tasks as we can afford
			if (minerals >= (*tmp)->getType().mineralPrice()
				&& gas >= (*tmp)->getType().gasPrice()
				&& (*tmp)->getLastOrder() <= Broodwar->getFrameCount()) // if true, it means we are going to build it right now (or very soon)
			{
				(*tmp)->update();
				minerals -= (*tmp)->getType().mineralPrice();
				gas -= (*tmp)->getType().gasPrice();
			}
		}
	}
	/// Check buildings in construction
	for (list<Unit*>::iterator it = inConstruction.begin();
		it != inConstruction.end(); )
	{
		list<Unit*>::iterator tmp = it++;
		if (!(*tmp)->isBeingConstructed())
		{
			inConstruction.erase(tmp);
			continue;
		}
		if ((*tmp)->isUnderAttack() && (*tmp)->getHitPoints() + (*tmp)->getShields() <= __MIN_HP_CANCEL_BUILDING_IN_CONSTRUCTION__)
		{
			addTask((*tmp)->getType(), (*tmp)->getTilePosition(), false, Broodwar->getFrameCount() + 2*Broodwar->getLatencyFrames() + 1); // TODO check if tmp works
			(*tmp)->cancelConstruction();
		}
	}

#ifdef __MACRO_DEBUG__
	Task::buildingPlacer->update();
	int dy = 0;
	for each (pTask t in tasks)
	{
		Broodwar->drawTextScreen(196, 325 - dy, "\x1f %s", t->getName().c_str());
		dy += 16;
	}
	if (!tasks.empty())
		Broodwar->drawBoxScreen(190, 325 - dy + 10, 360, 346, Colors::Cyan);
#endif

	/// Default check/defensive for the late game
	if (Broodwar->self()->supplyUsed() >= 396 && Broodwar->self()->minerals() > 1200 && Broodwar->self()->gas() > 600)
	{
		int nbGates = Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Gateway);
		int nbRobos = Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Robotics_Facility);
		if (nbGates < 12 && TheBuilder->willBuild(UnitTypes::Protoss_Gateway) + nbGates < 12)
			TheBuilder->build(UnitTypes::Protoss_Gateway);
		if (nbRobos < 3 && TheBuilder->willBuild(UnitTypes::Protoss_Robotics_Facility) + nbRobos < 3)
			TheBuilder->build(UnitTypes::Protoss_Robotics_Facility);
	}

}

const UnitType& Builder::nextBuildingType()
{
	if (tasks.empty() && !boTasks.empty())
		return boTasks.begin()->second->getType();
	else
		return tasks.front()->getType();
}

void Builder::onUnitCreate(Unit* unit)
{
	if (unit->getPlayer() == Broodwar->self() && unit->getType().isBuilding() && unit->isBeingConstructed())
		inConstruction.push_back(unit);
}

void Builder::onUnitDestroy(Unit* unit)
{
	if (unit->getType().isBuilding())
	{
		Task::buildingPlacer->onUnitDestroy(unit);
		if (unit->getType() == UnitTypes::Protoss_Photon_Cannon)
			build(unit->getType(), unit->getTilePosition()); // rebuild it
	}
}