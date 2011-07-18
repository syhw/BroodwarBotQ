#include <PrecompiledHeader.h>
#include <Macro/Base.h>
#include <Macro/Builder.h>
using namespace BWAPI;

std::set<Unit*> emptySet;

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
		std::set<Unit*> tmp = Broodwar->getUnitsOnTile(baseLocation->getTilePosition().x(), baseLocation->getTilePosition().y());
		for (std::set<Unit*>::const_iterator it = tmp.begin();
			it != tmp.end(); ++it)
		{
			if ((*it)->getType() == Broodwar->self()->getRace().getCenter())
				resourceDepot = *it;
		}
	}
	if (resourceDepot == NULL && !centerInConstruction)
		buildCenter();
	if (refinery == NULL)
	{	
		for (std::set<Unit*>::const_iterator it = baseLocation->getGeysers().begin();
		it != baseLocation->getGeysers().end(); ++it)
		{
			std::set<Unit*> tmp = Broodwar->getUnitsOnTile((*it)->getTilePosition.x(), (*it)->getTilePosition().y());
			for (std::set<Unit*>::const_iterator it2 = tmp->begin();
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
	if (refinery == NULL && !gasInConstruction)
		buildGas();
	ready = (resourceDepot && resourceDepot->exists() && (resourceDepot->isCompleted() || resourceDepot->getRemainingBuildTime()<300));
}

void Base::onUnitDestroy(BWAPI::Unit* u)
{
	if (u == refinery)
		refinery = NULL;
	if (u == resourceDepot)
		resourceDepot = NULL;
}

void Base::buildCenter()
{
	TheBuilder->build(Broodwar->self()->getRace().getCenter(), baseLocation->getTilePosition());
	centerInConstruction = true;
}

void Base::buildGas()
{
	for (std::set<Unit*>::const_iterator it = baseLocation->getGeysers().begin();
		it != baseLocation->getGeysers().end(); ++it)
		TheBuilder->build(Broodwar->self()->getRace().getRefinery(), (*it)->getTilePosition);
    gasInConstruction = true;
}

Base::Base(BWTA::BaseLocation* b)
: baseLocation(location)
, resourceDepot(NULL)
, refinery(NULL)
, active(false)
, ready(false)
, activeGas(false)
, centerInConstruction(false)
, gasInConstruction(false)
{
	buildCenter();
	if (getGas)
		buildGas();
}
