#pragma once
#include <Arbitrator.h>
#include <BWAPI.h>
#include <BuildManager.h>
#include <BuildOrderManager.h>
#include "CSingleton.h"
class SupplyManager: public CSingleton<SupplyManager>
{
	friend class CSingleton<SupplyManager>;
  public:
	void setDependencies(BuildManager* bm, BuildOrderManager* bom);
    void update();
    std::string getName() const;
    int getPlannedSupply() const;
    int getSupplyTime(int supplyCount) const;
    void setSeedPosition(BWAPI::TilePosition p);
    BuildManager* buildManager;
    BuildOrderManager* buildOrderManager;
    int lastFrameCheck;
    BWAPI::TilePosition seedPosition;
 private :
	   SupplyManager();
};