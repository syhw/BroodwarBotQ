#include <PrecompiledHeader.h>
#include <Macro/Macro.h>
#include <Macro/Base.h>
#include <Macro/Builder.h>
#include "Defines.h"

using namespace BWAPI;

std::set<Unit*> emptySet;

Base::Base(BWTA::BaseLocation* b, Unit* center)
: baseLocation(b)
, resourceDepot(center)
, refinery(NULL)
, ready(false)
, paused(false)
, activeGas(false)
, centerInConstruction(0)
, gasInConstruction(0)
, cannoned(false)
{
}

Base::Base(const Base& b)
: baseLocation(b.baseLocation)
, resourceDepot(b.resourceDepot)
, refinery(b.refinery)
, ready(b.ready)
, paused(b.paused)
, activeGas(b.activeGas)
, centerInConstruction(b.centerInConstruction)
, gasInConstruction(b.gasInConstruction)
{
}

void Base::onUnitDestroy(BWAPI::Unit* u)
{
	if (u == refinery)
		refinery = NULL;
	if (u == resourceDepot)
		resourceDepot = NULL;
}

BWTA::BaseLocation* Base::getBaseLocation() const
{
	return baseLocation;
}

BWAPI::Unit* Base::getResourceDepot() const
{
	return resourceDepot;
}

BWAPI::Unit* Base::getRefinery() const
{
	return refinery;
}

const std::set<BWAPI::Unit*>& Base::getMinerals() const
{
	if (baseLocation == NULL)
		return emptySet;
	return baseLocation->getMinerals();
}

const std::set<BWAPI::Unit*>& Base::getGeysers() const
{
	if (baseLocation == NULL)
		return emptySet;
	return baseLocation->getGeysers();
}

void Base::setActiveGas(bool gas)
{
	this->activeGas = gas;
}

void Base::setPaused(bool paused)
{
	this->paused = paused;
}

bool Base::isPaused() const
{
	return paused;
}

bool Base::isReady() const
{
	return ready;
}

bool Base::isActive() const
{
	return !paused && ready;
}

void Base::update()
{
#ifdef __MACRO_DEBUG__
	BWAPI::Broodwar->drawBoxMap(this->baseLocation->getPosition().x()-2*TILE_SIZE, this->baseLocation->getPosition().y()-2*TILE_SIZE,
		this->baseLocation->getPosition().x()+2*TILE_SIZE, this->baseLocation->getPosition().y()+2*TILE_SIZE, BWAPI::Colors::Cyan);
#endif
	if (resourceDepot == NULL)
	{
		std::set<Unit*> tmp = Broodwar->getUnitsOnTile(baseLocation->getTilePosition().x(), baseLocation->getTilePosition().y());
		for (std::set<Unit*>::const_iterator it = tmp.begin();
			it != tmp.end(); ++it)
		{
			if ((*it)->getPlayer() == Broodwar->self() && (*it)->getType() == Broodwar->self()->getRace().getCenter())
				resourceDepot = *it;
		}
	}
	if (resourceDepot == NULL && (Broodwar->getFrameCount() - centerInConstruction) > __MAX_TRIES_BUILD_SOMETHING__+250) // it has been canceled
	{
		/// TODO can be that there is a burrowed unit or a mine here
		buildCenter();
	}
	if (refinery == NULL)
	{	
		for (std::set<Unit*>::const_iterator it = baseLocation->getGeysers().begin();
		it != baseLocation->getGeysers().end(); ++it)
		{
			std::set<Unit*> tmp = Broodwar->getUnitsOnTile((*it)->getTilePosition().x(), (*it)->getTilePosition().y());
			for (std::set<Unit*>::const_iterator it2 = tmp.begin();
				it2 != tmp.end(); ++it2)
			{
				if ((*it2)->getType() == Broodwar->self()->getRace().getRefinery())
				{
					refinery = *it2;
					break;
				}
			}
		}
	}
	if (activeGas && refinery == NULL && (Broodwar->getFrameCount() - gasInConstruction) > __MAX_TRIES_BUILD_SOMETHING__+250)
		buildGas();
	ready = (resourceDepot && resourceDepot->exists() && (resourceDepot->isCompleted() || resourceDepot->getRemainingBuildTime()<300)); // 300 frames before completion
	if (Broodwar->getFrameCount() > __MINUTES_CANNONS_MINERALS__*60*24
		&& !cannoned
		&& !paused && ready) // active
	{
		TheBuilder->buildCannonsMinerals(baseLocation);
		cannoned = true;
	}

	/// TODO PURE HACK TO EXPAND WHEN AT SATURATION !!! !!! !!!
	if (resourceDepot != NULL && !(Broodwar->getFrameCount() % ((resourceDepot->getPosition().x() % 100) + 15)))
	{
		std::set<Unit*> tmp = Broodwar->getUnitsInRadius(resourceDepot->getPosition(), 8*TILE_SIZE);
		int nbWorkers = 0;
		int nbResources = 0;
		for each (Unit* u in tmp)
		{
			if (u->getPlayer() == Broodwar->self() && u->getType().isWorker())
				++nbWorkers;
			else if (u->getType().isMineralField() || u->getType() == UnitTypes::Resource_Vespene_Geyser)
				++nbResources;
		}
		double ratio = (static_cast<double>(nbWorkers) / nbResources);
		if (ratio > 3.0 || Broodwar->getFrameCount() > 12*60*24 && ratio > 2.0)
			Macro::Instance().expand();
	}
}

void Base::buildCenter()
{
	///TheBuilder->addTask(Broodwar->self()->getRace().getCenter(), baseLocation->getTilePosition(), false);
	TheBuilder->addTask(Broodwar->self()->getRace().getCenter(), baseLocation->getTilePosition(), true);
	centerInConstruction = Broodwar->getFrameCount();
}

void Base::buildGas()
{
	for (std::set<Unit*>::const_iterator it = baseLocation->getGeysers().begin();
		it != baseLocation->getGeysers().end(); ++it)
	{
		if (!TheBuilder->willBuild(Broodwar->self()->getRace().getRefinery())) // TODO correct (we can't build 2 Refineries at once)
			TheBuilder->build(Broodwar->self()->getRace().getRefinery(), (*it)->getTilePosition());
	}
    gasInConstruction = Broodwar->getFrameCount();
}
