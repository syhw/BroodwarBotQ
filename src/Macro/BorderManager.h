#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include "CSingleton.h"
class InformationManager;
class BorderManager : public CSingleton<BorderManager>
{
	friend class CSingleton<BorderManager>;
  public:
	void setDependencies(InformationManager * im);
    void addMyBase(BWTA::BaseLocation* location);
    void removeMyBase(BWTA::BaseLocation* location);
    const std::set<BWTA::Chokepoint*>& getMyBorder() const;
    const std::set<BWTA::Chokepoint*>& getEnemyBorder() const;
    const std::set<BWTA::Chokepoint*>& getMyRegions() const;
    const std::set<BWTA::Chokepoint*>& getEnemyRegions() const;
    void update();
  private:
	  BorderManager(){}
    void recalculateBorders();
    InformationManager* informationManager;
    std::set<BWTA::BaseLocation*> myBases;
    std::set<BWTA::BaseLocation*> enemyBases;
    std::set<BWTA::Region*> myRegions;
    std::set<BWTA::Region*> enemyRegions;
    std::set<BWTA::Chokepoint*> myBorder;
    std::set<BWTA::Chokepoint*> enemyBorder;
};