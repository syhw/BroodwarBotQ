#pragma once
#include <map>
#include "DefenseManager.h"
#include <set>
#include <BWAPI.h>
#include <Arbitrator.h>
#include <BWTA.h>
#include "Base.h"
#include "CSingleton.h"
class DefenseManager;
class BuildOrderManager;
class BorderManager;
class BaseManager : public CSingleton<BaseManager>
{
	friend class CSingleton<BaseManager>;
 public:
  void setDependencies();
  void update();
  void updateRefineries();
  Base* getBase(BWTA::BaseLocation* location);
  void expand(int priority = 100);
  void expand(BWTA::BaseLocation* location, int priority = 100);
  std::set<Base*> getActiveBases() const;
  std::set<Base*> getAllBases() const;
  std::set<Base*> getAllBasesWithGas();

  std::string getName();
  void onRemoveUnit(BWAPI::Unit* unit);

  bool hasRefinery(BWTA::BaseLocation* location);
  bool isRefineryNeeded();
  void setRefineryBuildPriority(int priority);
  int  getRefineryBuildPriority();
  BWTA::BaseLocation* naturalExpand;
 private:
  BaseManager();
  void addBase(BWTA::BaseLocation* location);
  void removeBase(BWTA::BaseLocation* location);
  BuildOrderManager* builder;
  BorderManager* borderManager;
  DefenseManager * defenseManager;
  std::map<BWTA::BaseLocation*,Base*> location2base;
  std::set<Base*> allBases;
  int RefineryNeeded;
  int refineryBuildPriority;
  void computeNatural();//Set the naturalExpandField
};