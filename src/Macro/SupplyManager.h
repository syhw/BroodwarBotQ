#pragma once
#include "Macro/Arbitrator.h"
#include <BWAPI.h>
#include "Macro/BuildManager.h"
#include "Macro/BuildOrderManager.h"
class SupplyManager
{
  public:
    SupplyManager();
    void setBuildManager(BuildManager* buildManager);
    void setBuildOrderManager(BuildOrderManager* buildOrderManager);
    void update();
    std::string getName() const;
    int getPlannedSupply() const;
    int getSupplyTime(int supplyCount) const;
    void setSeedPosition(BWAPI::TilePosition p);
    BuildManager* buildManager;
    BuildOrderManager* buildOrderManager;
    int lastFrameCheck;
    BWAPI::TilePosition seedPosition;
};