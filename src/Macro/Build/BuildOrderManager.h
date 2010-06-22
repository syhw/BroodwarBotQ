#pragma once
#include <CSingleton.h>
#include <Arbitrator.h>
#include <BWAPI.h>
#include <BuildManager.h>
#include <TechManager.h>
#include <UpgradeManager.h>
#include <ProductionManager.h>

class BuildOrderManager : public CSingleton<BuildOrderManager>
{
	friend class CSingleton<BuildOrderManager>;

	private:
		BuildOrderManager();
		~BuildOrderManager();

  public:
    class BuildItem
    {
      public:
        BWAPI::UnitType unitType;
        BWAPI::TechType techType;
        BWAPI::UpgradeType upgradeType;
        BWAPI::Position seedPosition;
        bool isAdditional;
        int count;
    };
    void update();
    std::string getName() const;
    void build(int count, BWAPI::UnitType t, int priority, BWAPI::TilePosition seedPosition=BWAPI::TilePositions::None);
    void buildAdditional(int count, BWAPI::UnitType t, int priority, BWAPI::TilePosition seedPosition=BWAPI::TilePositions::None);
    void research(BWAPI::TechType t, int priority);
    void upgrade(int level, BWAPI::UpgradeType t, int priority);
    bool hasResources(BWAPI::UnitType t);
    bool hasResources(BWAPI::TechType t);
    bool hasResources(BWAPI::UpgradeType t);
    void spendResources(BWAPI::UnitType t);
    void spendResources(BWAPI::TechType t);
    void spendResources(BWAPI::UpgradeType t);

	int getUnusedMinerals() {return BWAPI::Broodwar->self()->cumulativeMinerals()-this->usedMinerals;};
	int getUnusedGas() {return BWAPI::Broodwar->self()->cumulativeGas()-this->usedGas;};
	int getPlannedCount( BWAPI::UnitType unitType);

  private:
    BuildManager* buildManager;
    TechManager* techManager;
    UpgradeManager* upgradeManager;
	ProductionManager* productionManager;
    std::map<int, std::list<BuildItem> > items;
    int usedMinerals;
    int usedGas;
};