#pragma once
#include <map>
#include <set>
#include <BWAPI.h>
#include <Arbitrator.h>
#include <BWTA.h>
#include <BuildOrderManager.h>
#include "Base.h"
#include "BaseObject.h"


class BaseManager: public CSingleton<BaseManager>, public BaseObject
{
	friend class CSingleton<BaseManager>;

	private:
		BaseManager();
		~BaseManager();

  public:
  void setBuildOrderManager(BuildOrderManager* builder);
  void update();
  void addBase(BWTA::BaseLocation* location);
  void expand(BWTA::BaseLocation* location);
  std::set<Base*> getActiveBases() const;
  std::set<Base*> getAllBases() const;

  std::string getName();
  void onUnitDestroy(BWAPI::Unit* unit);

	// Qt interface
	virtual QWidget* createWidget(QWidget* parent) const;
	virtual void refreshWidget(QWidget* widget) const;

  BuildOrderManager* builder;
  std::map<BWTA::BaseLocation*,Base*> location2base;
  std::set<Base*> allBases;
};