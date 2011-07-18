#pragma once
#include <map>
#include <set>
#include <BWAPI.h>
#include "Macro/Arbitrator.h"
#include <BWTA.h>
#include <Macro/Base.h>
#include <Macro/BorderManager.h>

class BasesManager
{
public:
	static BasesManager* create();
	static void destroy();
	void setBorderManager(BorderManager* borderManager);
	void update();

	Base* getBase(BWTA::BaseLocation* location);

	Base* expand(BWTA::BaseLocation* location = NULL, bool getGas = true);

	const std::set<Base*>& getActiveBases() const;
	const std::set<Base*>& getReadyBases() const;
	const std::set<Base*>& getAllBases() const;
	const std::set<Base*>& getDestroyedBases() const;

	std::string getName();
	void onUnitDestroy(BWAPI::Unit* unit);

private:
	std::list<Base> bases;
	BasesManager();
	~BasesManager();
	BorderManager* borderManager;
	std::map<BWTA::BaseLocation*, Base*> location2base;
	std::set<Base*> activeBases;
	std::set<Base*> readyBases;
	std::set<Base*> allBases;
	std::set<Base*> destroyedBases;
};
extern BasesManager* TheBasesManager;