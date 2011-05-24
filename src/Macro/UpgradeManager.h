#pragma once
#include <Arbitrator.h>
#include <BWAPI.h>
#include <BuildingPlacer.h>
class UpgradeManager : public Arbitrator::Controller<BWAPI::Unit*,double>
{
  public:
    class Upgrade
    {
      public:
        BWAPI::UpgradeType type;
        int level;
    };
    UpgradeManager(Arbitrator::Arbitrator<BWAPI::Unit*,double>* arbitrator);
    void setBuildingPlacer(BuildingPlacer* placer);
    virtual void onOffer(std::set<BWAPI::Unit*> units);
    virtual void onRevoke(BWAPI::Unit* unit, double bid);
    virtual void update();
    virtual std::string getName() const;
    void onRemoveUnit(BWAPI::Unit* unit);
    bool upgrade(BWAPI::UpgradeType type, int level = -1);
    int getPlannedLevel(BWAPI::UpgradeType type) const;
    int getStartedLevel(BWAPI::UpgradeType type) const;
    int getCompletedLevel(BWAPI::UpgradeType type) const;

  private:
    Arbitrator::Arbitrator<BWAPI::Unit*,double>* arbitrator;
    BuildingPlacer* placer;
    std::map<BWAPI::UnitType,std::list< Upgrade > > upgradeQueues;
    std::map<BWAPI::Unit*,Upgrade> upgradingUnits;
    std::map<BWAPI::UpgradeType, int> plannedLevel;
    std::map<BWAPI::UpgradeType, int> startedLevel;
};