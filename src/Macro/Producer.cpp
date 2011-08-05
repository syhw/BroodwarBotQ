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
: _nbArchons(0)
, _nbDarkArchons(0)
{
	TheProducer = this;
}

Producer::~Producer()
{
	TheArbitrator->removeController(this);
	TheProducer = NULL;
}

/*** 
 * Check if we can produce the given UnitType w.r.t. producing structures
 * AND tech buildings. !!! Ask TheBuilder to build them otherwise. !!!
 */
bool Producer::checkCanProduce(UnitType t)
{
	if (t == UnitTypes::Protoss_Archon)
		return checkCanProduce(UnitTypes::Protoss_High_Templar);
	else if (t == UnitTypes::Protoss_Dark_Archon)
		return checkCanProduce(UnitTypes::Protoss_Dark_Templar);

	map<UnitType, int> needed = t.requiredUnits();
	bool ret = true;
	for (map<UnitType, int>::const_iterator it = needed.begin();
		it != needed.end(); ++it)
	{
		if (it->first.isBuilding()
			&& _techStructures.find(it->first) == _techStructures.end()
			&& _producingStructures.find(it->first) == _producingStructures.end()) // TODO Archons, or merge with checkHave
		{
			if (it->first.isBuilding() && !Broodwar->self()->incompleteUnitCount(it->first) && !TheBuilder->willBuild(it->first))
				TheBuilder->build(it->first);
			ret = false;
		}
	}
	return ret;
}

/*** 
 * Check if we can research the given TechType w.r.t. tech structures
 * !!! Ask TheBuilder to build them otherwise. !!!
 */
bool Producer::checkCanTech(TechType t)
{
    return checkHaveTech(t.whatResearches());
}

/*** 
 * Check if we can research the given UpgradeType w.r.t. tech structures
 * !!! Ask TheBuilder to build them otherwise. !!!
 */
bool Producer::checkCanUpgrade(UpgradeType t)
{
	return checkHaveTech(t.whatsRequired()) && checkHaveTech(t.whatUpgrades());
}

/***
 * Check if we have the given TECH UnitType and build it if we don't intend to
 */
bool Producer::checkHaveTech(UnitType ut)
{
	if (ut == UnitTypes::None)
		return true;
	if (_techStructures.find(ut) != _techStructures.end()) // TECH structures only at the moment
		return true;
	else
	{
		if (ut.isBuilding()
			&& !Broodwar->self()->incompleteUnitCount(ut) && !TheBuilder->willBuild(ut))
			TheBuilder->build(ut);
		return false;
	}
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
 * Produces the difference between wanted _number_ and existing units
 */
void Producer::produce(int number, BWAPI::UnitType t, int priority, int increment)
{
	int add = max(0, number - Broodwar->self()->completedUnitCount(t) - willProduce(t));
	produceAdditional(add, t, priority, increment);
}

/***
 * Try to always keep number units of type t (lowest priority)
 */
void Producer::produceAlways(int number, BWAPI::UnitType t, int increment)
{
	_wantedNumbers.insert(make_pair<UnitType, pair<int, int> >(t, make_pair<int, int>(number, increment)));
}

/***
 * Researches the TechType t
 */
void Producer::researchTech(BWAPI::TechType t)
{
	checkCanTech(t);
	for (list<TechType>::const_iterator it = _techsQueue.begin();
		it != _techsQueue.end(); ++it)
	{
		if (*it == t)
			return;
	}
	_techsQueue.push_back(t);
}

/***
 * Researches the UpgradeType t
 */
void Producer::researchUpgrade(BWAPI::UpgradeType t)
{
	checkCanUpgrade(t);
	for (list<UpgradeType>::const_iterator it = _upgradesQueue.begin();
		it != _upgradesQueue.end(); ++it)
	{
		if (*it == t)
			return;
	}
	_upgradesQueue.push_back(t);
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
	checkCanProduce(t);
	if (t == UnitTypes::Protoss_Archon)
	{
		_nbArchons += number;
		for (int i = 0; i < number*increment; i += increment)
		{
			_productionQueue.insert(make_pair<int, UnitType>(p + i, UnitTypes::Protoss_High_Templar));
			_productionQueue.insert(make_pair<int, UnitType>(p + i, UnitTypes::Protoss_High_Templar));
		}
	}
	else if (t == UnitTypes::Protoss_Dark_Archon)
	{
		_nbDarkArchons += number;
		for (int i = 0; i < number*increment; i += increment)
		{
			_productionQueue.insert(make_pair<int, UnitType>(p + i, UnitTypes::Protoss_Dark_Templar));
			_productionQueue.insert(make_pair<int, UnitType>(p + i, UnitTypes::Protoss_Dark_Templar));
		}
	}
	else
	{
		for (int i = 0; i < number*increment; i += increment)
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
	list<multimap<int, UnitType>::const_iterator > toRem;
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
				(frames - max(builder->second->getRemainingBuildTime(), builder->second->getRemainingTrainTime()))) // just a second one of the same unit, then it's too far in the future
			{
				supply += it->second.supplyRequired();
				minerals -= it->second.mineralPrice();
				gas -= it->second.gasPrice();
			}
			free.erase(builder);
			toRem.push_back(it);
		}
	}
	for each (multimap<int, UnitType>::const_iterator it in toRem)
		pq.erase(it);
	/// If, after all that, we have minerals left, we should build more production buildings
	multimap<int, UnitType>::const_iterator it = pq.begin();
	if (it != pq.end())
	{
		UnitType ut = it->second.whatBuilds().first;
		while (minerals >= ut.mineralPrice() && gas >= ut.gasPrice() && it != pq.end())
		{
			ut = it->second.whatBuilds().first;
			if (!TheBuilder->willBuild(ut)
				&& ut != Broodwar->self()->getRace().getCenter()) // we don't want to be building more Nexuses to produce more probes :D
			{
				_neededProductionBuildings.insert(ut);
				minerals -= ut.mineralPrice();
				gas -= ut.gasPrice();
			}
			++it;
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

/// Defensive check for if it exists already
/// Should not be required if we solve the problem of the first Nexus. Defensive is always good in alpha code anyway...
void Producer::addToProducingStructures(Unit* u)
{
	for (multimap<BWAPI::UnitType, ProducingUnit>::const_iterator it = _producingStructures.begin();
		it != _producingStructures.end(); ++it)
	{
		if (it->second.unit == u)
			return;
	}
	_producingStructures.insert(make_pair<UnitType, ProducingUnit>(u->getType(), u));
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

	/// Produce Archons if needed
	if ((_nbArchons || _nbDarkArchons) && Broodwar->getFrameCount() % 41)
		mergeArchons();
	/// Free merging archons if merged or taking too long
	freeMerging();

	/// Filter out all producing/tech structures in construction that just finished
	for (list<Unit*>::const_iterator it = _producingStructuresInConstruction.begin();
		it != _producingStructuresInConstruction.end(); )
	{
		if ((*it)->isCompleted())
		{
			addToProducingStructures(*it);
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
		int frames = min(180*24,
			Broodwar->self()->getRace().getSupplyProvider().buildTime()
			+ max((int)(Broodwar->self()->getRace().getSupplyProvider().mineralPrice() / TheResourceRates->getGatherRate().getMinerals()) // important only if we perfectly consume our resources
			, 10*24)); // should be the upper bound on the time to start building a pylon
		if (!TheBuilder->numberInFutureTasks(Broodwar->self()->getRace().getSupplyProvider()) &&
			Broodwar->self()->supplyTotal() < 200 &&
			additionalUnitsSupply(frames) + Broodwar->self()->supplyUsed() > TheBuilder->additionalSupplyNextFrames(frames) + Broodwar->self()->supplyTotal())
			TheBuilder->build(Broodwar->self()->getRace().getSupplyProvider());
	}

	int minerals = Broodwar->self()->minerals() - Macro::Instance().reservedMinerals;
	int gas = Broodwar->self()->gas() - Macro::Instance().reservedGas;

	if (Broodwar->getFrameCount() % 21
#ifdef __CONTROL_BO_UNTIL_SECOND_PYLON__
		&& (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Pylon) > 1 || Broodwar->getFrameCount() >= 5000)
#endif
		)
	{
		/// Research Techs/Upgrades
		if (!_techsQueue.empty() || !_upgradesQueue.empty())
		{
			for (multimap<UnitType, Unit*>::const_iterator it = _techStructures.begin();
				it != _techStructures.end(); ++it)
			{
				if (!it->second->isResearching() && !it->second->isUpgrading())
				{
					/// Techs
					for (list<TechType>::const_iterator itech = _techsQueue.begin();
						itech != _techsQueue.end(); )
					{
						if (itech->whatResearches() == it->first
							&& checkCanTech(*itech)
							&& minerals >= itech->mineralPrice()
							&& gas >= itech->gasPrice())
						{
							minerals -= itech->mineralPrice();
							gas -= itech->gasPrice();
							_researchingTech.insert(make_pair<Unit*, TechType>(it->second, *itech));
							it->second->research(*itech);
							_techsQueue.erase(itech++);
						}
						else
							++itech;
					}
					/// Upgrades
					for (list<UpgradeType>::const_iterator iupg = _upgradesQueue.begin();
						iupg != _upgradesQueue.end(); )
					{
						if (iupg->whatUpgrades() == it->first
							&& checkCanUpgrade(*iupg)
							&& minerals >= iupg->mineralPrice()
							&& gas >= iupg->gasPrice())
						{
							minerals -= iupg->mineralPrice();
							gas -= iupg->gasPrice();
							_researchingUpgrade.insert(make_pair<Unit*, UpgradeType>(it->second, *iupg));
							it->second->upgrade(*iupg);
							_upgradesQueue.erase(iupg++);
						}
						else
							++iupg;
					}
				}
			}
		}

		/// Build more producing structures
		list<UnitType> noLongerNeededProdBuildings;
		for (set<BWAPI::UnitType>::iterator it = _neededProductionBuildings.begin();
			it != _neededProductionBuildings.end(); ++it)
		{
			if (minerals > it->mineralPrice() && gas > it->gasPrice())
			{
				TheBuilder->build(*it);
				noLongerNeededProdBuildings.push_back(*it);
			}
		}
		for each (UnitType ut in noLongerNeededProdBuildings)
			_neededProductionBuildings.erase(ut);
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
	minerals = Broodwar->self()->minerals() - Macro::Instance().reservedMinerals;
	gas = Broodwar->self()->gas() - Macro::Instance().reservedGas;
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
					toRemove.push_back(it);
				}
				// erase builder from free in all cases (train or not)
				free.erase(builder);
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
	if (unit->getType().isBuilding())
		TheArbitrator->setBid(this, unit, 80);
	else if (_nbArchons > 0 && unit->getType() == UnitTypes::Protoss_High_Templar)
		TheArbitrator->setBid(this, unit, 96);
	else if (_nbDarkArchons > 0 && unit->getType() == UnitTypes::Protoss_Dark_Templar)
		TheArbitrator->setBid(this, unit, 96);
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
			if (_researchingTech.find(unit) != _researchingTech.end()
				&& !Broodwar->self()->hasResearched(_researchingTech[unit]))
				researchTech(_researchingTech[unit]);
			else if (_researchingUpgrade.find(unit) != _researchingUpgrade.end()
				&& (Broodwar->self()->getUpgradeLevel(_researchingUpgrade[unit])
				< _researchingUpgrade[unit].maxRepeats())) // not exact but will do
				researchUpgrade(_researchingUpgrade[unit]);
			_researchingTech.erase(unit);
			_researchingUpgrade.erase(unit);
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
				addToProducingStructures(*it);
			else
				_producingStructuresInConstruction.push_back(*it);
		}
		else if ((*it)->getType().isBuilding())
		{
			TheArbitrator->accept(this, *it, 95);
			if ((*it)->isCompleted())
				_techStructures.insert(make_pair<UnitType, Unit*>((*it)->getType(), *it));
			else
				_techStructuresInConstruction.push_back(*it);
		}
		else if ((*it)->getType() == UnitTypes::Protoss_High_Templar)
		{
			TheArbitrator->accept(this, *it, 96);
			_hts.push_back(*it);
		}
		else if ((*it)->getType() == UnitTypes::Protoss_Dark_Templar)
		{
			TheArbitrator->accept(this, *it, 96);
			_dts.push_back(*it);
		}
		else
		{
			TheArbitrator->decline(this, *it, 0);
			TheArbitrator->removeBid(this, *it);
		}
	}
}

void Producer::onRevoke(BWAPI::Unit* unit, double bid)
{
	if (unit->getType().isBuilding())
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

	if (unit->getType() == UnitTypes::Protoss_High_Templar
		|| unit->getType() == UnitTypes::Protoss_Dark_Archon)
	{
		_hts.remove(unit);
		_dts.remove(unit);
		_htsMerging.erase(unit);
		_dtsMerging.erase(unit);
	}
}

string Producer::getName() const
{
	return "Producer";
}

void Producer::mergeArchons()
{
	list<pair<Unit*, Unit*> > toMerge;
	/// Create pair of DTs/HTs
	while (_nbArchons > 0 && _hts.size() >= 2)
	{
		Unit* tmp = _hts.front();
		_hts.pop_front();
		toMerge.push_back(make_pair<Unit*, Unit*>(_hts.front(), tmp));
		_htsMerging.insert(make_pair<Unit*, int>(tmp, 0));
		_htsMerging.insert(make_pair<Unit*, int>(_hts.front(), 0));
		_hts.pop_front();
		--_nbArchons;
	}
	while (_nbDarkArchons > 0 && _dts.size() >= 2)
	{
		Unit* tmp = _dts.front();
		_dts.pop_front();
		toMerge.push_back(make_pair<Unit*, Unit*>(_dts.front(), tmp));
		_dtsMerging.insert(make_pair<Unit*, int>(tmp, 0));
		_dtsMerging.insert(make_pair<Unit*, int>(_dts.front(), 0));
		_dts.pop_front();
		--_nbDarkArchons;
	}
	/// Merge each pair
	for each (pair<Unit*, Unit*> p in toMerge)
	{
		if (p.first != NULL && p.first->exists()
			&& p.second != NULL && p.second->exists())
			p.first->useTech(TechTypes::Archon_Warp, p.second);
	}
}

void Producer::freeMerging()
{
	list<map<Unit*, int>::const_iterator> toRemHT;
	for (map<Unit*, int>::const_iterator it = _htsMerging.begin();
		it != _htsMerging.end(); ++it)
	{
		if (!it->first || !it->first->exists() || it->second > 30*24) // 30 seconds to merge
			toRemHT.push_back(it);
		else
			_htsMerging[it->first] += 1;
	}
	for each (map<Unit*, int>::const_iterator it in toRemHT)
		_htsMerging.erase(it);
	list<map<Unit*, int>::const_iterator> toRemDT;
	for (map<Unit*, int>::const_iterator it = _dtsMerging.begin();
		it != _dtsMerging.end(); ++it)
	{
		if (!it->first || !it->first->exists() || it->second > 30*24) // 30 seconds to merge
			toRemDT.push_back(it);
		else
			_dtsMerging[it->first] += 1;
	}
	for each (map<Unit*, int>::const_iterator it in toRemDT)
		_dtsMerging.erase(it);
}