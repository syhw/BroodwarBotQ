#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <set>

class Base
{
public:
	static Base* CreateBase(BWTA::BaseLocation* b, bool getGas = true);
	BWTA::BaseLocation* getBaseLocation() const;
	BWAPI::Unit* getResourceDepot() const;
	BWAPI::Unit* getRefinery() const;
	const std::set<BWAPI::Unit*>& getMinerals() const;
	const std::set<BWAPI::Unit*>& getGeysers() const;
	void setPaused(bool paused);
	bool isPaused() const;
	bool isReady() const;
	bool isActive() const;
	void update();

private:
	void buildCenter();
	void buildGas();
	Base(BWTA::BaseLocation* b);
	BWTA::BaseLocation* baseLocation;
	BWAPI::Unit* resourceDepot;
	BWAPI::Unit* refinery;
	bool ready;
	bool paused;
	bool active;
	bool activeGas;
	bool centerInConstruction;
	bool gasInConstruction;
};