#include <PrecompiledHeader.h>
#include "Macro/MacroBaseManager.h"
#include "Macro/MacroManager.h"
#include "Macro/BasicTaskExecutor.h"
#include "Macro/SimCityBuildingPlacer.h"
#include "Macro/TerminateIfEmpty.h"
#include "Macro/BasicWorkerFinder.h"
#include "Macro/Task.h"
using namespace BWAPI;

std::set<Unit*> emptySet;

BWTA::BaseLocation* MacroBase::getBaseLocation() const
{
	return baseLocation;
}

BWAPI::Unit* MacroBase::getResourceDepot() const
{
	return resourceDepot;
}

BWAPI::Unit* MacroBase::getRefinery() const
{
	return refinery;
}

const std::set<BWAPI::Unit*>& MacroBase::getMinerals() const
{
	if (baseLocation==NULL)
		return emptySet;
	return baseLocation->getMinerals();
}

const std::set<BWAPI::Unit*>& MacroBase::getGeysers() const
{
	if (baseLocation == NULL)
		return emptySet;
	return baseLocation->getGeysers();
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
	if (resourceDepot == NULL)
	{
		if (Broodwar->getUnitsOnTile(
			resourceDepot = 
	}
	if (refinery = NULL)
	{
		if (refinery_ts!=NULL)
			refinery = refinery_ts->getBuildUnit();
	}
	ready = (resourceDepot && resourceDepot->exists() && (resourceDepot->isCompleted() || resourceDepot->getRemainingBuildTime()<300));
}

void Base::onUnitDestroy(BWAPI::Unit* u)
{
	if (u == refinery)
		refinery = NULL;
	if (u == resourceDepot)
		resourceDepot = NULL;
}

Base::Base(BWTA::BaseLocation* b)
: baseLocation(location)
, resourceDepot(NULL)
, refinery(NULL)
, active(false)
, ready(false)
, activeGas(false)
{
	TheBuilder->build(Broodwar->self()->getRace().getCenter(), b->getTilePosition());
	if (getGas)
	{
		for (std::set<Unit*>::const_iterator it = b->getGeysers().begin();
			it != b->getGeysers().end(); ++it)
			TheBuilder->build(Broodwar->self()->getRace().getRefinery(), (*it)->getTilePosition);
	}
}
