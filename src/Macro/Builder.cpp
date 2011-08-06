#include <PrecompiledHeader.h>
#include "Macro/Macro.h"
#include "Macro/Builder.h"
#include "BWSAL.h"
#include "Macro/ReservedMap.h"
#include "Macro/UnitGroupManager.h"

using namespace BWAPI;
using namespace std;

#define __MIN_HP_CANCEL_BUILDING_IN_CONSTRUCTION__ 50
#define __MIN_FRAMES_TO_START_CONSTRUCTION__ 10*24
#define __MAX_TRIES_BUILD_SOMETHING__ 1440 // IN FRAMES, 1 minute here

SimCityBuildingPlacer* Task::buildingPlacer = NULL;

int Task::reservedMineralsNexus = 0; // TODO use that

Task::Task(BWAPI::Unit* w, BWAPI::TilePosition tp, BWAPI::UnitType ut, int lo)
: worker(w)
, tilePosition(tp)
, type(ut)
, lastOrder(lo)
, finished(false)
, tries(0)
{
}

Task::~Task()
{
	if (type == UnitTypes::Protoss_Nexus)
		Task::reservedMineralsNexus -= type.mineralPrice();

	Macro::Instance().reservedMinerals -= type.mineralPrice();
	Macro::Instance().reservedGas -= type.gasPrice();
	TheArbitrator->removeController(this);
}

int Task::framesToCompleteRequirements(UnitType type)
{
	int ret = 0;
	map<UnitType, int> rqU = type.requiredUnits();
	for (map<UnitType, int>::const_iterator it = rqU.begin();
		it != rqU.end(); ++it)
	{
		if (Broodwar->self()->completedUnitCount(it->first) < it->second)
		{
			/// We don't have the requirements
			/// we will have to delay this construction
			int delay = 0;
			if (it->first.isBuilding()
				&& Broodwar->self()->incompleteUnitCount(it->first) < it->second)
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

void Task::init()
{
	if (tilePosition == TilePositions::None)
		tilePosition = buildingPlacer->getTilePosition(type);
	if (!tilePosition.isValid() || TilePositions::None)
	{
		finished = true;
		return;
	}

	if (type == UnitTypes::Protoss_Nexus)
		Task::reservedMineralsNexus += type.mineralPrice();

	Macro::Instance().reservedMinerals += type.mineralPrice();
	Macro::Instance().reservedGas += type.gasPrice();
	int delay = Task::framesToCompleteRequirements(type); // launches the requirements
	if (delay)
		lastOrder = max(lastOrder, Broodwar->getFrameCount() + delay);
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

void Task::buildIt()
{
	if (tries > __MAX_TRIES_BUILD_SOMETHING__)
	{
		finished = true;
#ifdef __DEBUG__
		Broodwar->printf("\x08 !!! CANCELED Building %s at (%d,%d) !!!", type.getName().c_str(), tilePosition.x(), tilePosition.y());
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
	if (tilePosition == TilePositions::None)
		buildingPlacer->getTilePosition(type);
	if (Broodwar->getFrameCount() > lastOrder + 17 + Broodwar->getLatencyFrames())
	{
		/// If it requires psi but there is not, ask for a powering pylon, or cancel if we can't power it
		if (type != UnitTypes::Protoss_Pylon && type != UnitTypes::Protoss_Assimilator && type != UnitTypes::Protoss_Nexus
			&& !Broodwar->hasPower(tilePosition)) // type.requiresPsi()
		{
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
				finished = true;
			return;
		}
		/// Move closer to the construction site
		if (worker->getPosition().getApproxDistance(Position(tilePosition)) > 6 * TILE_SIZE) // 6 > sqrt(biggest^2+biggest^2) (biggest buildings as Protoss are nexus/gates)
		{
			if (type == UnitTypes::Protoss_Nexus)
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
		{
#ifdef __DEBUG__
			Broodwar->printf("Can't build %s at (%d,%d)", type.getName().c_str(), tilePosition.x(), tilePosition.y());
#endif
		}
		lastOrder = Broodwar->getFrameCount();
	}
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
			(*it)->move(Position(Broodwar->self()->getStartLocation())); // try and move the unit, TODO will block if the unit doesn't move
		else if (!(tmp == UnitTypes::Resource_Vespene_Geyser && type == Broodwar->self()->getRace().getRefinery()))
		{
			tilePosition = buildingPlacer->getTilePosition(type); // really blocked (can't move)
			break;
		}
	}
}

void Task::update()
{
	if (Broodwar->getFrameCount() <= lastOrder) // delay hack
		return;

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
void Builder::addTask(const UnitType& t, const TilePosition& seedPosition, int lastOrder)
{
	pTask tmp(new Task(NULL, seedPosition, t, lastOrder));
	tmp->init();
	tasks.push_back(tmp);
}

void Builder::build(const BWAPI::UnitType& t, const BWAPI::TilePosition& seedPosition)
{
	if (t == Broodwar->self()->getRace().getCenter())
		TheBasesManager->expand();
	else
		addTask(t, seedPosition);
}

void Builder::buildOrder(const BWAPI::UnitType& t, int supplyAsTime, const BWAPI::TilePosition& seedPosition)
{
	pTask tmp(new Task(NULL, seedPosition, t));
	boTasks.insert(make_pair<int, pTask>(supplyAsTime, tmp));
}

/// Attempts to build 2 pylons + 2 cannons around the minerals
void Builder::buildCannonsMinerals(BWTA::BaseLocation* b)
{
	if (b == NULL)
		return;
	Task::buildingPlacer->makeCannonsMinerals(b);
	// 2 pylons
	TilePosition tp = Task::buildingPlacer->getTilePosition(UnitTypes::Protoss_Pylon, b->getTilePosition());
	if (tp == TilePositions::None)
		return;
	addTask(UnitTypes::Protoss_Pylon, tp);
	tp = Task::buildingPlacer->getTilePosition(UnitTypes::Protoss_Pylon, b->getTilePosition());
	if (tp == TilePositions::None)
		return;
	addTask(UnitTypes::Protoss_Pylon, tp);
	// 2 cannons
	tp = Task::buildingPlacer->getTilePosition(UnitTypes::Protoss_Photon_Cannon, b->getTilePosition());
	if (tp == TilePositions::None)
		return;
	addTask(UnitTypes::Protoss_Pylon, tp, Broodwar->getFrameCount() + UnitTypes::Protoss_Pylon.buildTime() + __MIN_FRAMES_TO_START_CONSTRUCTION__);
	tp = Task::buildingPlacer->getTilePosition(UnitTypes::Protoss_Photon_Cannon, b->getTilePosition());
	if (tp == TilePositions::None)
		return;
	addTask(UnitTypes::Protoss_Pylon, tp, Broodwar->getFrameCount() + UnitTypes::Protoss_Pylon.buildTime() + __MIN_FRAMES_TO_START_CONSTRUCTION__);
}

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
	int number = 0;
	for each (Unit* u in inConstruction)
		if (u->getType() == t)
			++number;
	return number;
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
#ifdef __DEBUG__
	Broodwar->drawTextScreen(190, 56, "\x11 incS: %d", supply);
#endif
	return supply;
}

/***
* It should be the last update()/onFrame() call (to move "blocking construction" units)
*/
void Builder::update()
{
	/// Follow the BO
	if (!boTasks.empty())
	{
		list<int> toRemove;
		for (multimap<int, pTask>::iterator it = boTasks.begin();
			it != boTasks.end(); ++it)
		{
			if (it->first <= Broodwar->self()->supplyUsed()/2)
			{
#ifdef __DEBUG__
				Broodwar->printf("Building %s pop %d", it->second->getName().c_str(), Broodwar->self()->supplyUsed()/2);
#endif
				tasks.push_front(it->second);
				it->second->init();
				toRemove.push_back(it->first);
			}
		}
		for (list<int>::const_iterator it = toRemove.begin();
			it != toRemove.end(); ++it)
			boTasks.erase(*it);
	}
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
			addTask((*tmp)->getType(), (*tmp)->getTilePosition(), Broodwar->getFrameCount() + 2*Broodwar->getLatencyFrames() + 1); // TODO check if tmp works
			(*tmp)->cancelConstruction();
		}
	}

#ifdef __DEBUG__
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
		if (nbGates < 16 && TheBuilder->willBuild(UnitTypes::Protoss_Gateway) + nbGates < 16)
			TheBuilder->build(UnitTypes::Protoss_Gateway);
		if (nbRobos < 4 && TheBuilder->willBuild(UnitTypes::Protoss_Robotics_Facility) + nbRobos < 16)
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