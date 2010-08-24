#ifndef SCOUTMANAGER__H
#define SCOUTMANAGER__H

#pragma once
#include <CSingleton.h>
#include <Arbitrator.h>
#include <BWAPI.h>
#include <Regions.h>
#include <BWTA.h>
#include "Goal.h"
#include "Subgoal.h"
#include "FindEnemyGoal.h"
#include "SeeSubgoal.h"
#include "FindSubgoal.h"
#include "WarManager.h"
#include "BaseObject.h"
#include "ExploreGoal.h"
class GoalManager;



class GoalManager;

class ScoutManager : public Arbitrator::Controller<BWAPI::Unit*,double>, public CSingleton<ScoutManager>, public BaseObject
{
	friend class CSingleton<ScoutManager>;

public:
	void setDependencies();
	virtual void update();
	virtual void onOffer(std::set<BWAPI::Unit*> units);
    virtual void onRevoke(BWAPI::Unit* unit, double bid);
	virtual std::string getName() const;
	// Goals // Just ideas, not yet implemented
	void scoutAllEnemies();
	void counterWall();
	void counterBuild();
	void harassWorkers();
	void checkEmptyXP();

	//TODO positionsToSurvey; // to refresh infos about enemy bases: lord(zerg), scan/vessel?(terran), obs(toss)
	void onUnitCreate(BWAPI::Unit* unit);//New 
	void onUnitShow(BWAPI::Unit* unit);
	void findEnemy();//New

#ifdef BW_QT_DEBUG
	// Qt interface
	virtual QWidget* createWidget(QWidget* parent) const;
	virtual void refreshWidget(QWidget* widget) const;
#endif
private:

	ScoutManager();
	~ScoutManager();
	UnitsGroup* findUnitsGroup(pGoal goal); //Match a UG with the goal (call setUnitsGroup and addGoal)
	bool to_remove;
    void updateScoutAssignments();
	WarManager * warManager;
	Arbitrator::Arbitrator<BWAPI::Unit*,double>* arbitrator;
	Regions* regions;
	std::list<pGoal> awaitingGoals;
	bool exploringEnemy;
};
#endif 