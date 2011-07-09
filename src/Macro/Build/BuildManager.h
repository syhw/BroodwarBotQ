#pragma once
#include <Arbitrator.h>
#include <BWAPI.h>
#include "CSingleton.h"
class BuildingPlacer;
class ConstructionManager;
class ProductionManager;
class MorphManager;
class BuildManager : public CSingleton<BuildManager>
{
	friend class CSingleton<BuildManager>;
  public:
	void setDependencies();
	BuildingPlacer* getBuildingPlacer();
	void update();
    std::string getName() const;
    void onRemoveUnit(BWAPI::Unit* unit);
    bool build(BWAPI::UnitType type);
    bool build(BWAPI::UnitType type, bool forceNoAddon);
    bool build(BWAPI::UnitType type, BWAPI::TilePosition goalPosition);
    bool build(BWAPI::UnitType type, BWAPI::TilePosition goalPosition, bool forceNoAddon);
    int getPlannedCount(BWAPI::UnitType type) const;
    int getStartedCount(BWAPI::UnitType type) const;
    int getCompletedCount(BWAPI::UnitType type) const;
    void setBuildDistance(int distance);
    BWAPI::UnitType getBuildType(BWAPI::Unit* unit) const;
    void setDebugMode(bool debugMode);

  private:
	 BuildManager();
    ~BuildManager();
    Arbitrator::Arbitrator<BWAPI::Unit*,double>* arbitrator;
    BuildingPlacer* buildingPlacer;
    ConstructionManager* constructionManager;
    ProductionManager* productionManager;
    MorphManager* morphManager;
    bool debugMode;
};