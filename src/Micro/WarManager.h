#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <Utils/CSingleton.h>
#include <Arbitrator.h>
#include "InformationManager.h"

class UnitsGroup;
class ScoutController;

class WarManager: public CSingleton<WarManager>, public Arbitrator::Controller<BWAPI::Unit*,double>
{
	friend class CSingleton<WarManager>;
    BWTA::BaseLocation* home;
	WarManager();
	~WarManager();
	std::list<UnitsGroup *> promptedRemove;
	bool remove(UnitsGroup* u);
    InformationManager* informationManager;
public:
	std::list<UnitsGroup*> unitsGroups;
	virtual void onOffer(std::set<BWAPI::Unit*> units);
	virtual void onRevoke(BWAPI::Unit* unit, double bid);
	virtual std::string getName() const;
	virtual void update();
	void onUnitCreate(BWAPI::Unit* unit);
	void onUnitDestroy(BWAPI::Unit* unit);
	void sendGroupToAttack( UnitsGroup* ug);
	void sendGroupToDefense( UnitsGroup* ug);
	void promptRemove(UnitsGroup* ug);//Guarantee that on the nextFrame :
	//-The target of the units of this UG will be their position so they are idling
	//-The unitsgroup will be removed from unitsGroups and deleted
};

