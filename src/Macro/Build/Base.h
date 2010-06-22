#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <set>
class Base
{
  public:
  Base(BWTA::BaseLocation* location);
  BWTA::BaseLocation* getBaseLocation() const;
  BWAPI::Unit* getResourceDepot() const;
  const std::set<BWAPI::Unit*>& getMinerals() const;
  const std::set<BWAPI::Unit*>& getGeysers() const;
  bool isActive() const;
  bool isBeingConstructed() const;

  void setResourceDepot(BWAPI::Unit* unit);
  void setActive(bool active);

	unsigned int exploitedGeysers;

private:
  BWTA::BaseLocation* baseLocation;
  BWAPI::Unit* resourceDepot;
  std::set<BWAPI::Unit*> minerals;
  bool active;
  bool beingConstructed;
};