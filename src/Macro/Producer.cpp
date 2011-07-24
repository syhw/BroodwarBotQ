#include <PrecompiledHeader.h>
#include "Macro/Producer.h"
#include "Macro/Macro.h"
#include "Macro/UnitGroupManager.h"
#include "BWSAL.h"

using namespace BWAPI;
using namespace std;

ProducingUnit::ProducingUnit(Unit* u)
: unit(u)
, lastAction(-24)
{
}

ProducingUnit::ProducingUnit(const ProducingUnit& pu)
: unit(pu.unit)
, lastAction(pu.lastAction)
{
}

Unit* ProducingUnit::operator->() const
{
	return unit;
}

void ProducingUnit::train(UnitType t)
{
	if (Broodwar->getFrameCount() - lastAction > 2*Broodwar->getLatencyFrames() + 1)
	{
		this->lastAction = Broodwar->getFrameCount();
		unit->train(t);
	}
}

Producer* TheProducer = NULL;

Producer* Producer::create()
{
	if (TheProducer)
		return TheProducer;
	TheProducer = new Producer();
	return TheProducer;
}

void Producer::destroy()
{
	if (TheProducer)
		delete TheProducer;
}

Producer::Producer()
{
	TheProducer = this;
}

Producer::~Producer()
{
	TheArbitrator->removeAllBids(this);
	TheProducer = NULL;
}

/*** 
 * Check if we can produce the given UnitType w.r.t. producing structures
 * AND tech buildings. Ask TheBuilder to build them otherwise.
 */
bool Producer::checkCanProduce(UnitType t)
{
	map<UnitType, int> needed = t.requiredUnits();
	bool ret = true;
	for (map<UnitType, int>::const_iterator it = needed.begin();
		it != needed.end(); ++it)
	{
		if (_techStructures.find(it->first) == _techStructures.end()
			&& _producingStructures.find(it->first) == _producingStructures.end()) // TODO Archons
		{
			if (it->first.isBuilding() && !TheBuilder->willBuild(it->first))
				TheBuilder->build(it->first);
			ret = false;
		}
	}
	return ret;
}

void Producer::produce(int number, BWAPI::UnitType t, int priority, int increment)
{
	int add = max(0, number - SelectAll(t).size());
	produceAdditional(add, t, priority, increment);
}

/***
 * Produce at most *number* units of type *t* with this *priority* 
 * decreasing with *increment* for each unit produced
 * For instance: 40 goons 20 zealots with twice as much goons as zealots:
 * produce(20, UnitTypes::Protoss_Zealot, 80, 2)
 * produce(40, UnitTypes::Protoss_Dragoon, 80) (increment = 1 is implied)
 * Add 6 templars and then 1 templar every 5 goons (at most, if we're still producing goons) until 10 templars:
 * produce(10, UnitTypes::Protoss_High_Templar, 86, 5)
 */
void Producer::produceAdditional(int number, BWAPI::UnitType t, int priority, int increment)
{
	if (number <= 0) // safety
		return;
	int p = 100 - priority; // (multi)maps are order by increasing order
	increment = max(1, increment); // safety
	for (int i = 0; i < number*increment; i += increment)
	{
		checkCanProduce(t);
		_productionQueue.insert(make_pair<int, UnitType>(p + i, t));
	}
}

void Producer::update()
{
	if (_producingStructures.empty()) // hack for the start (to add the first Nexus) because units does not exist in the cstor
	{
		UnitType centerType = Broodwar->self()->getRace().getCenter();
		set<Unit*> centers = SelectAll(centerType);
		for (set<Unit*>::const_iterator it = centers.begin();
			it != centers.end(); ++it)
			_producingStructures.insert(make_pair<UnitType, ProducingUnit>(centerType, *it));
	}

	multimap<UnitType, ProducingUnit*> free;
	for (multimap<UnitType, ProducingUnit>::iterator it = _producingStructures.begin();
		it != _producingStructures.end(); ++it)
	{
		if (Broodwar->getFrameCount() - it->second.lastAction > Broodwar->getLatencyFrames()
			&& (it->second->isIdle() || 
			(it->second->getRemainingTrainTime() <= Broodwar->getLatencyFrames() && it->second->getTrainingQueue().size() <= 1)))
			free.insert(make_pair<UnitType, ProducingUnit*>(it->first, &(it->second)));
	}
	if (free.empty())
		return;

	// launch new units productions
    int rM = Macro::Instance().reservedMinerals;
	int rG = Macro::Instance().reservedGas;
	int minerals = Broodwar->self()->minerals();
	int gas = Broodwar->self()->gas();
	list<multimap<int, UnitType>::const_iterator> toRemove;
	for (multimap<int, UnitType>::const_iterator it = _productionQueue.begin();
		it != _productionQueue.end(); ++it)
	{
		if (checkCanProduce(it->second))
		{
			multimap<UnitType, ProducingUnit*>::iterator builder = free.find(it->second.whatBuilds().first);
			if (builder != free.end() // TODO Archons
				&& minerals - rM >= it->second.mineralPrice()
				&& gas - rG >= it->second.gasPrice()
				&& Broodwar->self()->supplyTotal() - Broodwar->self()->supplyUsed () >= it->second.supplyRequired())
			{
				builder->second->train(it->second);
				minerals -= it->second.mineralPrice();
				gas -= it->second.gasPrice();
				free.erase(builder);
				toRemove.push_back(it);
			}
		}
	
	}
	// remove train orders that have been passed from the _productionQueue
	for each (multimap<int, UnitType>::const_iterator it in toRemove)
		_productionQueue.erase(it);
}

void Producer::onUnitCreate(BWAPI::Unit* unit)
{
	if (unit->getPlayer() != Broodwar->self())
		return;
	if (unit->getType().isBuilding() && unit->getType().canProduce())
		TheArbitrator->setBid(this, unit, 80);
	else if (unit->getType() == UnitTypes::Protoss_Cybernetics_Core
		|| unit->getType() == UnitTypes::Protoss_Robotics_Support_Bay
		|| unit->getType() == UnitTypes::Protoss_Citadel_of_Adun
		|| unit->getType() == UnitTypes::Protoss_Fleet_Beacon
		|| unit->getType() == UnitTypes::Protoss_Templar_Archives
		|| unit->getType() == UnitTypes::Protoss_Observatory
		|| unit->getType() == UnitTypes::Protoss_Arbiter_Tribunal)
		_techStructures.insert(make_pair<UnitType, Unit*>(unit->getType(), unit));
}

void Producer::onUnitDestroy(BWAPI::Unit* unit)
{
	for (multimap<UnitType, ProducingUnit>::const_iterator it = _producingStructures.begin();
		it != _producingStructures.end(); ++it)
	{
		if (it->second.unit == unit)
		{
			_producingStructures.erase(it);
			return;
		}
	}
	for (multimap<UnitType, Unit*>::const_iterator it = _techStructures.begin();
		it != _techStructures.end(); ++it)
	{
		if (it->second == unit)
		{
			_techStructures.erase(it);
			return;
		}
	}
}

void Producer::onOffer(set<Unit*> units)
{
	for (set<Unit*>::const_iterator it = units.begin();
		it != units.end(); ++it)
	{
		if ((*it)->getType().canProduce())
		{
			TheArbitrator->accept(this, *it, 95);
			_producingStructures.insert(make_pair<UnitType, ProducingUnit>((*it)->getType(), *it));
		}
		else
			TheArbitrator->decline(this, *it, 0);
	}
}

void Producer::onRevoke(BWAPI::Unit* unit, double bid)
{
	for (multimap<UnitType, ProducingUnit>::iterator it = _producingStructures.begin();
		it != _producingStructures.end(); ++it)
	{
		if (it->second.unit == unit)
		{
			_producingStructures.erase(it);
			return;
		}
	}
}

string Producer::getName() const
{
	return "Producer";
}
