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
  BWAPI::Unit* getRefinery() const;
  std::set<BWAPI::Unit*> getMinerals() const;
  std::set<BWAPI::Unit*> getGeysers() const;
  bool isActive() const;
  bool isActiveGas() const;
  bool isBeingConstructed() const;

  bool hasGas() const;

  void setResourceDepot(BWAPI::Unit* unit);
  void setRefinery(BWAPI::Unit* unit);
  void setActive(bool active);
  void setActiveGas(bool active);

private:
  BWTA::BaseLocation* baseLocation;
  BWAPI::Unit* resourceDepot;
  BWAPI::Unit* refinery;
  std::set<BWAPI::Unit*> minerals;
  bool active;
  bool activeGas;
  bool beingConstructed;
};