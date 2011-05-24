#pragma once
#include <Arbitrator.h>
#include <BWAPI.h>
#include <BuildingPlacer.h>
class ProductionManager : public Arbitrator::Controller<BWAPI::Unit*,double>
{
  public:
    ProductionManager(Arbitrator::Arbitrator<BWAPI::Unit*,double>* arbitrator, BuildingPlacer* placer);
    virtual void onOffer(std::set<BWAPI::Unit*> units);
    virtual void onRevoke(BWAPI::Unit* unit, double bid);
    virtual void update();
    virtual std::string getName() const;
    virtual std::string getShortName() const;

    void onRemoveUnit(BWAPI::Unit* unit);
    bool train(BWAPI::UnitType type, bool forceNoAddon=false);
    int getPlannedCount(BWAPI::UnitType type) const;
    int getStartedCount(BWAPI::UnitType type) const;
    BWAPI::UnitType getBuildType(BWAPI::Unit* unit) const;

  private:
    class ProductionUnitType
    {
      public:
        BWAPI::UnitType type;
        bool forceNoAddon;
    };
    class Unit
    {
      public:
        ProductionUnitType type;
        int lastAttemptFrame;
        BWAPI::Unit* unit;
        bool started;
    };
    bool canMake(BWAPI::Unit* builder, BWAPI::UnitType type);
    Arbitrator::Arbitrator<BWAPI::Unit*,double>* arbitrator;
    std::map<BWAPI::UnitType,std::list<ProductionUnitType > > productionQueues;
    std::map<BWAPI::Unit*,Unit> producingUnits;
    BuildingPlacer* placer;
    std::map<BWAPI::UnitType, int> plannedCount;
    std::map<BWAPI::UnitType, int> startedCount;
};