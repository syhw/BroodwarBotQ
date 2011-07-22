#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <set>

class Base
{
public:
	Base(BWTA::BaseLocation* b, BWAPI::Unit* center=NULL);
    Base(const Base& b);
	void Base::onUnitDestroy(BWAPI::Unit* u);
	BWTA::BaseLocation* getBaseLocation() const;
	BWAPI::Unit* getResourceDepot() const;
	BWAPI::Unit* getRefinery() const;
	const std::set<BWAPI::Unit*>& getMinerals() const;
	const std::set<BWAPI::Unit*>& getGeysers() const;
	void setActiveGas(bool gas);
	void setPaused(bool paused);
	bool isPaused() const;
	bool isReady() const;
	bool isActive() const;
	void update();

private:
	void buildCenter();
	void buildGas();
	BWTA::BaseLocation* baseLocation;
	BWAPI::Unit* resourceDepot;
	BWAPI::Unit* refinery;
	bool ready;
	bool paused;
	bool activeGas;
	bool centerInConstruction;
	bool gasInConstruction;
};