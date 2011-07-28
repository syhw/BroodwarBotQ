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

bool ProducingUnit::train(UnitType t)
{
	if (Broodwar->getFrameCount() - lastAction > 2*Broodwar->getLatencyFrames() + 1
		&& unit->canIssueCommand(UnitCommand::train(unit, t)))
	{
		this->lastAction = Broodwar->getFrameCount();
		unit->train(t);
		return true;
	}
	return false;
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

int Producer::willProduce(UnitType t)
{
	int count = 0;
	for (multimap<int, UnitType>::const_iterator it = _productionQueue.begin();
		it != _productionQueue.end(); ++it)
		if (it->second == t)
			++count;
	return count;
}

/***
 * Produce the difference between wanted _number_ and existing units
 */
void Producer::produce(int number, BWAPI::UnitType t, int priority, int increment)
{
	int add = max(0, number - Broodwar->self()->completedUnitCount(t) - willProduce(t));
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

/***
 * Quick/gross estimate of how many additional supply we would need for
 * _frames_ frames later, for at more a production batch.
 * + side effect on _neededProductionBuildings
 * Should perhaps be done with events (maintaining an "plan" datastructure)
 */
int Producer::additionalUnitsSupply(int frames)
{
	int supply = 0;

	/// see which buildings can/may produce in the future time _frames_
	multimap<UnitType, Unit*> free;
	for (multimap<UnitType, ProducingUnit>::const_iterator it = _producingStructures.begin();
		it != _producingStructures.end(); ++it)
	{
		if (it->second->isIdle() || it->second->getRemainingTrainTime() <= frames)
			free.insert(make_pair<UnitType, Unit*>(it->first, it->second.unit));
	}
	for (list<Unit*>::const_iterator it = _producingStructuresInConstruction.begin();
		it != _producingStructuresInConstruction.end(); ++it)
	{
		if ((*it)->getRemainingBuildTime() < frames && (*it)->getType().canProduce())
			free.insert(make_pair<UnitType, Unit*>((*it)->getType(), (*it)));
	}

	/// Simulate building units from our _productionQueue
	multimap<int, BWAPI::UnitType> pq = _productionQueue; // copy because we're going to damage it :)
	int minerals = Broodwar->self()->minerals() - Macro::Instance().reservedMinerals 
		+ (int)(TheResourceRates->getGatherRate().getMinerals()*frames); // pessimistic interpolation (without economic growth)
	int gas = Broodwar->self()->gas() - Macro::Instance().reservedGas 
		+ (int)(TheResourceRates->getGatherRate().getGas()*frames);
	for (multimap<int, UnitType>::const_iterator it = pq.begin();
		it != pq.end(); ++it)
	{
		multimap<UnitType, Unit*>::iterator builder = free.find(it->second.whatBuilds().first);
		if (builder != free.end()
			&& minerals >= it->second.mineralPrice()
			&& gas >= it->second.gasPrice())
		{
			supply += it->second.supplyRequired();
			minerals -= it->second.mineralPrice();
			gas -= it->second.gasPrice();
			if (it->second.buildTime() < 
				(frames - max(builder->second->getRemainingBuildTime(), builder->second->getRemainingTrainTime()))) // just a second unit, then it's too far in the future
			{
				supply += it->second.supplyRequired();
				minerals -= it->second.mineralPrice();
				gas -= it->second.gasPrice();
			}
			free.erase(builder);
		}
	}
#ifdef __DEBUG__
	Broodwar->drawTextScreen(130, 56, "\x11 addS: %d,", supply);
#endif
	return supply;
}

const UnitType& Producer::mostSaturatedUT()
{
	return UnitTypes::None;
}

void Producer::update()
{
	/// hack for the start (to add the first Nexus) because units does not exist in the cstor
	if (_producingStructures.empty())
	{
		UnitType centerType = Broodwar->self()->getRace().getCenter();
		set<Unit*> centers = SelectAll(centerType);
		for (set<Unit*>::const_iterator it = centers.begin();
			it != centers.end(); ++it)
			_producingStructures.insert(make_pair<UnitType, ProducingUnit>(centerType, *it));
	}

	/// Filter out all producing/tech structures in construction that just finished
	for (list<Unit*>::const_iterator it = _producingStructuresInConstruction.begin();
		it != _producingStructuresInConstruction.end(); )
	{
		if ((*it)->isCompleted())
		{
			_producingStructures.insert(make_pair<UnitType, ProducingUnit>((*it)->getType(), *it));
			_producingStructuresInConstruction.erase(it++);
		}
		else
			++it;
	}
	for (list<Unit*>::const_iterator it = _techStructuresInConstruction.begin();
		it != _techStructuresInConstruction.end(); )
	{
		if ((*it)->isCompleted())
		{
			_techStructures.insert(make_pair<UnitType, Unit*>((*it)->getType(), *it));
			_techStructuresInConstruction.erase(it++);
		}
		else
			++it;
	}

	/// Organize/order supply to avoid supply block
	if (Broodwar->getFrameCount() % 27 
#ifdef __CONTROL_BO_UNTIL_SECOND_PYLON__
		&& (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Pylon) > 1 || Broodwar->getFrameCount() > 5000) // so that we can drive the early BO
#endif
		&& TheResourceRates->getGatherRate().getMinerals() > 0.00001) // TODO change
	{
		int frames = max(180*24,
			Broodwar->self()->getRace().getSupplyProvider().buildTime()
			+ (int)(Broodwar->self()->getRace().getSupplyProvider().mineralPrice() / TheResourceRates->getGatherRate().getMinerals()) // important only if we perfectly consume our resources
			+ 5*24); // should be the upper bound on the time to start building a pylon
		if (!TheBuilder->numberInFutureTasks(Broodwar->self()->getRace().getSupplyProvider()) &&
			Broodwar->self()->supplyTotal() < 200 &&
			additionalUnitsSupply(frames) + Broodwar->self()->supplyUsed() > TheBuilder->additionalSupplyNextFrames(frames) + Broodwar->self()->supplyTotal())
			TheBuilder->build(Broodwar->self()->getRace().getSupplyProvider());
	}

    /// Find buildings free to produce units
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
	{
		if (Broodwar->getFrameCount() % 23)
		{
			UnitType tmp = mostSaturatedUT();
			if (tmp != UnitTypes::None)
				TheBuilder->build(tmp);
		}
		return;
	}
	/// Launch new units productions
	int minerals = Broodwar->self()->minerals() - Macro::Instance().reservedMinerals;
	int gas = Broodwar->self()->gas() - Macro::Instance().reservedGas;
	list<multimap<int, UnitType>::const_iterator> toRemove;
	for (multimap<int, UnitType>::const_iterator it = _productionQueue.begin();
		it != _productionQueue.end(); ++it)
	{
		if (checkCanProduce(it->second))
		{
			multimap<UnitType, ProducingUnit*>::iterator builder = free.find(it->second.whatBuilds().first);
			if (builder != free.end() // TODO Archons
				&& minerals >= it->second.mineralPrice()
				&& gas >= it->second.gasPrice()
				&& Broodwar->self()->supplyTotal() - Broodwar->self()->supplyUsed () >= it->second.supplyRequired())
			{
				if (builder->second->train(it->second))
				{
					minerals -= it->second.mineralPrice();
					gas -= it->second.gasPrice();
					free.erase(builder);
				// erase builder from free in all cases (train or not)?
					toRemove.push_back(it);
				}
			}
		}
	
	}
	/// Remove train orders that have been passed from the _productionQueue
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
		_techStructuresInConstruction.push_back(unit);
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
	_producingStructuresInConstruction.remove(unit);
	_techStructuresInConstruction.remove(unit);
}

void Producer::onOffer(set<Unit*> units)
{
	for (set<Unit*>::const_iterator it = units.begin();
		it != units.end(); ++it)
	{
		if ((*it)->getType().canProduce())
		{
			TheArbitrator->accept(this, *it, 95);
			if ((*it)->isCompleted())
				_producingStructures.insert(make_pair<UnitType, ProducingUnit>((*it)->getType(), *it));
			else
				_producingStructuresInConstruction.push_back(*it);
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
	_producingStructuresInConstruction.remove(unit);
}

string Producer::getName() const
{
	return "Producer";
}
