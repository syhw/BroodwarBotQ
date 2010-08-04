#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <windows.h>
#include <CSingleton.h>
#include <Arbitrator.h>
#include <stdlib.h>
#include "BaseObject.h"


class UnitsGroup;
class Regions;
class ScoutManager;

class WarManager: public CSingleton<WarManager>, public Arbitrator::Controller<BWAPI::Unit*,double>, public BaseObject
{
	friend class CSingleton<WarManager>;

private:
	WarManager();
	~WarManager();
	std::list<UnitsGroup *> promptedRemove;
    void checkDefense();
	bool remove(UnitsGroup* u);
	Arbitrator::Arbitrator<BWAPI::Unit*,double>* arbitrator;
	Regions* regions;
public:
	void checkDefense();
	void setDependencies(Arbitrator::Arbitrator<BWAPI::Unit*,double>* arb, Regions * reg);
	void onStart();
	std::list<UnitsGroup*> unitsgroups;
	UnitsGroup* defgroup;
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
	//-The unitsgroup will be removed from unitsgroups and deleted


#ifdef BW_QT_DEBUG
	// Qt interface
	virtual QWidget* createWidget(QWidget* parent) const;
	virtual void refreshWidget(QWidget* widget) const;
#endif

	static std::set<BWAPI::Unit*> getEnemies();

};
