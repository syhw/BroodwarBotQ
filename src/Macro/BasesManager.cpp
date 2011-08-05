#include <PrecompiledHeader.h>
#include "Macro/BasesManager.h"
#include "Macro/BorderManager.h"

using namespace BWAPI;

BasesManager* TheBasesManager = NULL;

BasesManager* BasesManager::create()
{
	if (TheBasesManager)
		return TheBasesManager;
	TheBasesManager = new BasesManager();
	return TheBasesManager;
}

void BasesManager::destroy()
{
	if (TheBasesManager)
		delete TheBasesManager;
}

BasesManager::BasesManager()
: firstGasPop(12)
, expanding(false)
{
	TheBasesManager = this;
}

BasesManager::~BasesManager()
{
	for each (Base* b in allBases)
		delete b;
	TheBasesManager = NULL;
}

void BasesManager::update()
{
	//check to see if any new base locations need to be added
	for (std::set<BWTA::BaseLocation*>::const_iterator it = BWTA::getBaseLocations().begin();
		it != BWTA::getBaseLocations().end(); ++it)
	{
		BWTA::BaseLocation* location = *it;
		if (location2base.find(location) == location2base.end())
		{
			TilePosition tile = location->getTilePosition();
			for each (Unit* u in Broodwar->getUnitsOnTile(tile.x(), tile.y()))
				if (u->getPlayer() == Broodwar->self() && u->getType().isResourceDepot())
				{
					allBases.push_back(new Base(location, u));
					location2base[location] = allBases.back();
					TheBorderManager->addMyBase(location);
				}
		}
	}

	if (allBases.front()->getRefinery() == NULL && Broodwar->self()->supplyUsed()/2 >= firstGasPop)
		allBases.front()->setActiveGas(true);

	for each(Base* mb in allBases)
	{
		mb->update();
		if (mb->isActive())
			activeBases.insert(mb);
		else
			activeBases.erase(mb);
		if (mb->isReady())
			readyBases.insert(mb);
		else
			readyBases.erase(mb);
		if (readyBases.size() == allBases.size())
			expanding = false;
	}

	if (Broodwar->self()->minerals() > 1200 // TODO remove
		&& !expanding)
		expand();
}

Base* BasesManager::getBase(BWTA::BaseLocation* location)
{
	std::map<BWTA::BaseLocation*, Base*>::iterator i = location2base.find(location);
	if (i == location2base.end())
		return NULL;
	return i->second;
}

void BasesManager::expand(BWTA::BaseLocation* location)
{
	if (expanding)
		return;
	else
		expanding = true;
	if (location == NULL)
	{
		// Find closer expand location not taken
		BWTA::BaseLocation* home = BWTA::getStartLocation(BWAPI::Broodwar->self());
		double minDist = -1;
		for(std::set<BWTA::BaseLocation*>::const_iterator i = BWTA::getBaseLocations().begin();
			i != BWTA::getBaseLocations().end(); i++)
		{
			double dist = home->getGroundDistance(*i);
			if (dist > 0 && getBase(*i) == NULL)
			{
				if (minDist < 0 || dist < minDist)
				{
					minDist = dist;
					location = *i;
				}
			}
		}
	}
#ifdef __DEBUG__
	if (location == NULL)
		Broodwar->printf("CANNOT EXPAND");
#endif

	if (location != NULL)
	{
		allBases.push_back(new Base(location));
		location2base[location] = allBases.back();
		TheBorderManager->addMyBase(location);
	}
}

void BasesManager::setFirstGasPop(int pop)
{
	firstGasPop = pop;
}

const std::set<Base*>& BasesManager::getActiveBases() const
{
	return activeBases;
}

const std::set<Base*>& BasesManager::getReadyBases() const
{
	return readyBases;
}

const std::list<Base*>& BasesManager::getAllBases() const
{
	return allBases;
}

std::set<BWTA::Region*> BasesManager::getRegionsBases() const
{
	std::set<BWTA::Region*> ret;
	for each (Base* b in allBases)
		ret.insert(b->getBaseLocation()->getRegion());
	return ret;
}

const std::set<Base*>& BasesManager::getDestroyedBases() const
{
	return destroyedBases;
}

std::string BasesManager::getName()
{
	return "BasesManager";
}

void BasesManager::onUnitDestroy(BWAPI::Unit* unit)
{
	for each(Base* b in allBases)
		b->onUnitDestroy(unit);
}

