#pragma once
#include <CSingleton.h>
#include <Arbitrator.h>
#include <BWAPI.h>
#include <BuildingPlacer.h>
class ConstructionManager : public Arbitrator::Controller<BWAPI::Unit*,double>, public CSingleton<ConstructionManager>
{
	friend class CSingleton<ConstructionManager>;

	private:
		ConstructionManager();
		~ConstructionManager();

  public:
    class Building
    {
      public:
        BWAPI::TilePosition goalPosition;
        BWAPI::TilePosition tilePosition;
        BWAPI::Position position;
        BWAPI::UnitType type;
        BWAPI::Unit* buildingUnit;
        BWAPI::Unit* builderUnit;
        int lastOrderFrame;
        bool started;
    };
    virtual void onOffer(std::set<BWAPI::Unit*> units);
    virtual void onRevoke(BWAPI::Unit* unit, double bid);
    virtual void update();
    virtual std::string getName() const;

    void onUnitDestroy(BWAPI::Unit* unit);
    bool build(BWAPI::UnitType type, BWAPI::TilePosition goalPosition);
    int getPlannedCount(BWAPI::UnitType type) const;
    int getStartedCount(BWAPI::UnitType type) const;

  private:
    Arbitrator::Arbitrator<BWAPI::Unit*,double>* arbitrator;
    BuildingPlacer* placer;
    std::map<BWAPI::Unit*,Building*> builders;
    std::list<Building> incompleteBuildings;
    std::map<BWAPI::UnitType,std::set<Building*> > buildingsNeedingBuilders;
    std::map<BWAPI::UnitType, int> plannedCount;
    std::map<BWAPI::UnitType, int> startedCount;
};