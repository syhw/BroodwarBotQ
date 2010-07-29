#include <BaseManager.h>
Base::Base(BWTA::BaseLocation* location)
{
  this->baseLocation     = location;
  this->resourceDepot    = NULL;
  this->refinery         = NULL;
  this->active           = false;
  this->activeGas        = false;
  this->beingConstructed = false;
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

bool Base::isBeingConstructed() const
{
  return this->beingConstructed;
}

bool Base::hasGas() const
{
  bool gas = false;

  //if Baselocation has a vespene geyser
  if(!(this->getBaseLocation()->isMineralOnly()))
  {
    gas = true;
  }
  return gas;
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
