#pragma once
#include <Arbitrator.h>
#include <CSingleton.h>
#include <BWAPI.h>
#include <BuildingPlacer.h>
class ProductionManager : public Arbitrator::Controller<BWAPI::Unit*,double>, public CSingleton<ProductionManager>
{
	friend class CSingleton<ProductionManager>;

	private:
		ProductionManager();
		~ProductionManager();

  public:
    class Unit
    {
      public:
        BWAPI::UnitType type;
        int lastAttemptFrame;
        BWAPI::Unit* unit;
        bool started;
    };
    virtual void onOffer(std::set<BWAPI::Unit*> units);
    virtual void onRevoke(BWAPI::Unit* unit, double bid);
    virtual void update();
    virtual std::string getName() const;

    void onUnitDestroy(BWAPI::Unit* unit);
    bool train(BWAPI::UnitType type);
    int getPlannedCount(BWAPI::UnitType type) const;
    int getStartedCount(BWAPI::UnitType type) const;

  private:
    Arbitrator::Arbitrator<BWAPI::Unit*,double>* arbitrator;
    std::map<BWAPI::UnitType,std::list<BWAPI::UnitType> > productionQueues; // <prod building, list<queued units> >
    std::map<BWAPI::Unit*,Unit> producingUnits; // building map that are producing
    BuildingPlacer* placer;
    std::map<BWAPI::UnitType, int> plannedCount;
    std::map<BWAPI::UnitType, int> startedCount;
};