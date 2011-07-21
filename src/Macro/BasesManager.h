#pragma once
#include <map>
#include <set>
#include <BWAPI.h>
#include <BWTA.h>
#include "Macro/Arbitrator.h"
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
	void expand(BWTA::BaseLocation* location = NULL);
	void setFirstGasPop(int pop);

	const std::list<Base*>& getAllBases() const;
	const std::set<Base*>& getActiveBases() const;
	const std::set<Base*>& getReadyBases() const;
	const std::set<Base*>& getDestroyedBases() const;

	std::string getName();
	void onUnitDestroy(BWAPI::Unit* unit);

private:
	int firstGasPop;
	BasesManager();
	~BasesManager();
	std::list<Base*> allBases;
	std::map<BWTA::BaseLocation*, Base*> location2base;
	std::set<Base*> activeBases;
	std::set<Base*> readyBases;
	std::set<Base*> destroyedBases;
};
extern BasesManager* TheBasesManager;