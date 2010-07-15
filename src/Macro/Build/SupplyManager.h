#pragma once
#include <CSingleton.h>
#include <Arbitrator.h>
#include <BWAPI.h>
#include <BuildManager.h>
#include <BuildOrderManager.h>
#include "BaseObject.h"


class SupplyManager : public CSingleton<SupplyManager>, public BaseObject
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

#ifdef BW_QT_DEBUG
    // Qt interface
    virtual QWidget* createWidget(QWidget* parent) const;
    virtual void refreshWidget(QWidget* widget) const;
#endif

    BuildManager* buildManager;
    BuildOrderManager* buildOrderManager;
    int lastFrameCheck;
};