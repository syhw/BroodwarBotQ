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

SimCityBuildingPlacer* Task::buildingPlacer = NULL;

Task::Task(BWAPI::Unit* w, BWAPI::TilePosition tp, BWAPI::UnitType ut, int lo)
: worker(w)
, tilePosition(tp)
, type(ut)
, lastOrder(lo)
, finished(false)
{
}

Task::~Task()
{
	Macro::Instance().reservedMinerals -= type.mineralPrice();
	Macro::Instance().reservedGas -= type.gasPrice();
	TheArbitrator->removeAllBids(this);
}

void Task::init()
{
	if (tilePosition == TilePositions::None)
		tilePosition = buildingPlacer->getTilePosition(type);
	Macro::Instance().reservedMinerals += type.mineralPrice();
	Macro::Instance().reservedGas += type.gasPrice();
	map<UnitType, int> rqU = type.requiredUnits();
	for (map<UnitType, int>::const_iterator it = rqU.begin();
		it != rqU.end(); ++it)
	{
		if (Broodwar->self()->completedUnitCount(it->first) < it->second)
		{
			TheBuilder->build(it->first); // could recurse on the last addition to the Task list but it's evil
			if (!lastOrder)
				lastOrder += Broodwar->getFrameCount();
			lastOrder += it->first.buildTime() + __MIN_FRAMES_TO_START_CONSTRUCTION__;
		}
	}
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
		double dist = 100000000.0;
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
	if (Broodwar->getFrameCount() > lastOrder + 27 + Broodwar->getLatencyFrames() && worker && worker->exists())
	{
		if (worker->getPosition().getApproxDistance(Position(tilePosition)) > 4 * TILE_SIZE)
		{
			worker->move(Position(tilePosition));
		}
		else if (!worker->build(tilePosition, type))
		{
#ifdef __DEBUG__
			Broodwar->printf("Can't build %s", type.getName().c_str());
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

void Task::update()
{
	if (Broodwar->getFrameCount() <= lastOrder) // delay hack
		return;

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
	if (Broodwar->self()->minerals() < type.mineralPrice() - 24 // TODO complete with rates (distance*rate/speed)
		&& Broodwar->self()->gas() < type.gasPrice() - 15)
		return;
	else if (worker == NULL || !worker->exists())
		askWorker();
	else
		buildIt();
}

const UnitType& Task::getType()
{
	return type;
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
	for (list<pTask>::iterator it = tasks.begin();
		it != tasks.end();)
	{
		list<pTask>::iterator tmp = it++;
		if ((*tmp)->isFinished())
			tasks.erase(tmp);
		else
			(*tmp)->update();
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
	Task::buildingPlacer->onUnitDestroy(unit);
}