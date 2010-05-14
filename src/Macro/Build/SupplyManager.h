#pragma once
#include <CSingleton.h>
#include <Arbitrator.h>
#include <BWAPI.h>
#include <BuildManager.h>
#include <BuildOrderManager.h>
class SupplyManager : public CSingleton<SupplyManager>
{
	friend class CSingleton<SupplyManager>;

	private:
		SupplyManager();
		~SupplyManager();

  public:
    void setBuildManager(BuildManager* buildManager);
    void setBuildOrderManager(BuildOrderManager* buildOrderManager);
    void update();
    std::string getName() const;
    int getPlannedSupply() const;
    BuildManager* buildManager;
    BuildOrderManager* buildOrderManager;
    int lastFrameCheck;
};