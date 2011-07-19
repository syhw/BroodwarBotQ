#include <PrecompiledHeader.h>
#include "Macro/Builder.h"
#include "BWSAL.h"
#include "Macro/ReservedMap.h"
#include "Macro/UnitGroupManager.h"

using namespace BWAPI;
using namespace std;

SimCityBuildingPlacer* Task::buildingPlacer = NULL;

Task::Task(BWAPI::Unit* w, BWAPI::TilePosition tp, BWAPI::UnitType ut)
	: worker(w)
	, tilePosition(tp)
	, type(ut)
	, lastOrder(0)
	, finished(false)
{
}

void Task::onOffer(set<Unit*> units)
{
	worker = NULL;
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
			TheArbitrator->decline(this, *it, 0);
	}
}

void Task::onRevoke(BWAPI::Unit* unit, double bid)
{
	if (unit == worker)
	{
		askWorker();
	}
}

void Task::askWorker()
{
	std::set<Unit*> usefulUnits = 
		SelectAll()(isWorker)(isCompleted)
		.not(isCarryingMinerals,isCarryingGas,isGatheringGas);
	TheArbitrator->setBid(this, usefulUnits, 60); // TODO change 60
}

void Task::buildIt()
{
	if (Broodwar->getFrameCount() > lastOrder + 47 && worker && worker->exists())
	{
		worker->build(tilePosition, type);
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

void Task::update()
{
	if (tilePosition == TilePositions::None)
		tilePosition = buildingPlacer->getTilePosition(type);
	else if (worker == NULL)
		askWorker();
	else
		buildIt();
	for (set<Unit*>::const_iterator it = Broodwar->getUnitsOnTile(tilePosition.x(), tilePosition.y()).begin();
		it != Broodwar->getUnitsOnTile(tilePosition.x(), tilePosition.y()).end(); ++it)
	{
		if ((*it)->getType() == type)
		{
			TheArbitrator->removeBid(this, worker);
			finished = true;
			return;
		}
	}
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
	TheBuilder = NULL;
}

void Builder::build(BWAPI::UnitType t, BWAPI::TilePosition seedPosition)
{
	tasks.push_back(Task(NULL, seedPosition, t));
}

void Builder::buildOrder(BWAPI::UnitType t, int supplyAsTime, BWAPI::TilePosition seedPosition)
{
    boTasks.insert(make_pair<int, Task>(supplyAsTime, Task(NULL, seedPosition, t)));
}

void Builder::update()
{
	/// Follow the BO
    if (!boTasks.empty())
	{
		list<int> toRemove;
		for (multimap<int, Task>::iterator it = boTasks.begin();
			it != boTasks.end(); ++it)
		{
			if (it->first <= Broodwar->self()->supplyUsed())
			{
				tasks.push_front(it->second);
				toRemove.push_back(it->first);
			}
		}
		for (list<int>::const_iterator it = toRemove.begin();
			it != toRemove.end(); ++it)
			boTasks.erase(*it);
	}
	/// Perform tasks
	for (list<Task>::iterator it = tasks.begin();
		it != tasks.end();)
	{
		list<Task>::iterator tmp = it++;
		if (tmp->isFinished())
			tasks.erase(tmp);
		else
			tmp->update();
	}
}