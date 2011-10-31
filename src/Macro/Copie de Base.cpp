#include <PrecompiledHeader.h>
#include <Macro/Base.h>

Base::Base(BWTA::BaseLocation* location)
: baseLocation(location)
, resourceDepot(NULL)
, refinery(NULL)
, active(false)
, ready(false)
, activeGas(false)
{
}

BWTA::BaseLocation* Base::getBaseLocation() const
{
	return this->baseLocation;
}

BWAPI::Unit* Base::getResourceDepot() const
{
	return this->resourceDepot;
}

BWAPI::Unit* Base::getRefinery() const
{
	return this->refinery;
}

std::set<BWAPI::Unit*> Base::getMinerals() const
{
	return this->baseLocation->getMinerals();
}

std::set<BWAPI::Unit*> Base::getGeysers() const
{
	return this->baseLocation->getGeysers();
}

bool Base::isActive() const
{
	return this->active;
}

bool Base::isActiveGas() const
{
	return this->activeGas;
}

bool Base::hasGas() const
{
	return !this->getBaseLocation()->isMineralOnly()
}

void Base::setResourceDepot(BWAPI::Unit* unit)
{
	this->resourceDepot = unit;
}

void Base::setRefinery(BWAPI::Unit* unit)
{
	this->refinery = unit;
}

void Base::setActive(bool active)
{
	this->active = active;
}

void Base::setActiveGas(bool active)
{
	this->activeGas = active;
}
