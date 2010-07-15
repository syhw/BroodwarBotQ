#pragma once
#include <CSingleton.h>
#include <Arbitrator.h>
#include <BWAPI.h>
#include "BaseObject.h"

class BuildingPlacer;
class ConstructionManager;
class ProductionManager;
class MorphManager;
class BuildManager : public CSingleton<BuildManager>, public BaseObject
{
	friend class CSingleton<BuildManager>;

	private:
		BuildManager();
		~BuildManager();

	public:
    void update();
    std::string getName() const;
    BuildingPlacer* getBuildingPlacer() const;
    void onUnitDestroy(BWAPI::Unit* unit);
    bool build(BWAPI::UnitType type);
    bool build(BWAPI::UnitType type, BWAPI::TilePosition goalPosition);
    int getPlannedCount(BWAPI::UnitType type) const;
    int getStartedCount(BWAPI::UnitType type) const;
    int getCompletedCount(BWAPI::UnitType type) const;
    void setBuildDistance(int distance);

#ifdef BW_QT_DEBUG
    // Qt interface
    virtual QWidget* createWidget(QWidget* parent) const;
    virtual void refreshWidget(QWidget* widget) const;
#endif

  private:
		Arbitrator::Arbitrator<BWAPI::Unit*,double>* arbitrator;
    BuildingPlacer* buildingPlacer;
    ConstructionManager* constructionManager;
    ProductionManager* productionManager;
    MorphManager* morphManager;
};
