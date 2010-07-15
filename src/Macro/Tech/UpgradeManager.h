#pragma once
#include <Arbitrator.h>
#include <BWAPI.h>
#include <BuildingPlacer.h>
#include "BaseObject.h"


class UpgradeManager : public Arbitrator::Controller<BWAPI::Unit*,double>, public CSingleton<UpgradeManager>, public BaseObject
{
	friend class CSingleton<UpgradeManager>;
	private:
    UpgradeManager();
		
  public:
    class Upgrade
    {
      public:
        BWAPI::UpgradeType type;
        int level;
    };
    void setBuildingPlacer(BuildingPlacer* placer);
    virtual void onOffer(std::set<BWAPI::Unit*> units);
    virtual void onRevoke(BWAPI::Unit* unit, double bid);
    virtual void update();
    virtual std::string getName() const;
    void onUnitDestroy(BWAPI::Unit* unit);
    bool upgrade(BWAPI::UpgradeType type);
    int getPlannedLevel(BWAPI::UpgradeType type) const;
    int getStartedLevel(BWAPI::UpgradeType type) const;
    int getCompletedLevel(BWAPI::UpgradeType type) const;

#ifdef BW_QT_DEBUG
    // Qt interface
    virtual QWidget* createWidget(QWidget* parent) const;
    virtual void refreshWidget(QWidget* widget) const;
#endif

  private:
    Arbitrator::Arbitrator<BWAPI::Unit*,double>* arbitrator;
    BuildingPlacer* placer;
    std::map<BWAPI::UnitType,std::list< Upgrade > > upgradeQueues;
    std::map<BWAPI::Unit*,Upgrade> upgradingUnits;
    std::map<BWAPI::UpgradeType, int> plannedLevel;
    std::map<BWAPI::UpgradeType, int> startedLevel;
};