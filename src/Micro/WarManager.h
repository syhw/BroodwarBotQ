#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <windows.h>
#include <CSingleton.h>
#include <Arbitrator.h>
#include <stdlib.h>
#include "BaseObject.h"
#include "InformationManager.h"

class UnitsGroup;
class ScoutManager;

class WarManager: public CSingleton<WarManager>, public Arbitrator::Controller<BWAPI::Unit*,double>, public BaseObject
{
	friend class CSingleton<WarManager>;

private:
    BWTA::BaseLocation* home;
	WarManager();
	~WarManager();
	std::list<UnitsGroup *> promptedRemove;
	bool remove(UnitsGroup* u);
	Arbitrator::Arbitrator<BWAPI::Unit*,double>* arbitrator;
    InformationManager* informationManager;
public:
	void setDependencies();
	void onStart();
	std::list<UnitsGroup*> unitsGroups;
	virtual void onOffer(std::set<BWAPI::Unit*> units);
	virtual void onRevoke(BWAPI::Unit* unit, double bid);
	virtual std::string getName() const;
	virtual void update();
	void onUnitCreate(BWAPI::Unit* unit);
	void onUnitDestroy(BWAPI::Unit* unit);
	void display();
	void sendGroupToAttack( UnitsGroup* ug);
	void sendGroupToDefense( UnitsGroup* ug);
	void promptRemove(UnitsGroup* ug);//Guarantee that on the nextFrame :
	//-The target of the units of this UG will be their position so they are idling
	//-The unitsgroup will be removed from unitsGroups and deleted

	static std::set<BWAPI::Unit*> getEnemies();

};

