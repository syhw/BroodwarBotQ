#include "BaseManager.h"
Base::Base(BWTA::BaseLocation* location)
{
  this->baseLocation     = location;
  this->resourceDepot    = NULL;
  this->active           = false;
  this->beingConstructed = false;
	this->exploitedGeysers = 0;
}

BWTA::BaseLocation* Base::getBaseLocation() const
{
  return this->baseLocation;
}

BWAPI::Unit* Base::getResourceDepot() const
{
  return this->resourceDepot;
}

const std::set<BWAPI::Unit*>& Base::getMinerals() const
{
  return this->baseLocation->getMinerals();
}

const std::set<BWAPI::Unit*>& Base::getGeysers() const
{
  return this->baseLocation->getGeysers();
}
bool Base::isActive() const
{
  return this->active;
}
bool Base::isBeingConstructed() const
{
  return this->beingConstructed;
}

void Base::setResourceDepot(BWAPI::Unit* unit)
{
  this->resourceDepot = unit;
}
void Base::setActive(bool active)
{
  this->active = active;
}
