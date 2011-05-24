#pragma once
#include <map>
#include <set>
#include <BWAPI.h>
#include <Arbitrator.h>
#include <BWTA.h>
#include "Base.h"
class BuildOrderManager;
class BorderManager;
class BaseManager
{
  public:
  BaseManager();
  void setBuildOrderManager(BuildOrderManager* builder);
  void setBorderManager(BorderManager* borderManager);
  void update();
  void updateRefineries();
  Base* getBase(BWTA::BaseLocation* location);
  BWTA::BaseLocation* expand(int priority = 100);
  BWTA::BaseLocation* expand(BWTA::BaseLocation* location, int priority = 100);
  std::set<Base*> getActiveBases() const;
  std::set<Base*> getAllBases() const;
  std::set<Base*> getAllBasesWithGas();

  std::string getName();
  void onRemoveUnit(BWAPI::Unit* unit);

  bool hasRefinery(BWTA::BaseLocation* location);
  bool isRefineryNeeded();
  void setRefineryBuildPriority(int priority);
  int  getRefineryBuildPriority();

  private:
  void addBase(BWTA::BaseLocation* location);
  void removeBase(BWTA::BaseLocation* location);
  BuildOrderManager* builder;
  BorderManager* borderManager;
  std::map<BWTA::BaseLocation*,Base*> location2base;
  std::set<Base*> allBases;
  int RefineryNeeded;
  int refineryBuildPriority;
};