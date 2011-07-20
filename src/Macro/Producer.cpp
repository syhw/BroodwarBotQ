#include <PrecompiledHeader.h>
#include "Macro/Producer.h"
#include "Macro/UnitGroupManager.h"
#include "BWSAL.h"

using namespace BWAPI;
using namespace std;

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
	TheProducer = NULL;
}

void Producer::produce(int number, BWAPI::UnitType t, int priority)
{
}

void Producer::update()
{
}

void Producer::onUnitCreate(BWAPI::Unit* unit)
{
	if (unit->getPlayer() == Broodwar->self() && unit->getType().canProduce())
		TheArbitrator->setBid(this, unit, 80);
}

void Producer::onUnitDestroy(BWAPI::Unit* unit)
{
	for (multimap<UnitType, Unit*>::const_iterator it = producingStructures.begin();
		it != producingStructures.end(); ++it)
	{
		if (it->second == unit)
		{
			producingStructures.erase(it);
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
			TheArbitrator->accept(this, *it, 90);
			producingStructures.insert(make_pair<UnitType, Unit*>((*it)->getType(), *it));
		}
		else
			TheArbitrator->decline(this, *it, 0);
	}
}

void Producer::onRevoke(BWAPI::Unit* unit, double bid)
{
	for (multimap<UnitType, Unit*>::iterator it = producingStructures.begin();
		it != producingStructures.end(); ++it)
	{
		if (it->second == unit)
		{
			producingStructures.erase(it);
			return;
		}
	}
}

string Producer::getName() const
{
	return "Producer";
}
