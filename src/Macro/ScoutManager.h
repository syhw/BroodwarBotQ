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
#include "ScoutGoal.h"
#include "SeeSubgoal.h"
#include "FindSubgoal.h"
#include "MicroManager.h"
class GoalManager;



class ScoutManager :  public CSingleton<ScoutManager>
{
	friend class CSingleton<ScoutManager>;

private:
		ScoutManager();
		~ScoutManager();
public:
	virtual void update();
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
	void exploreRegion(BWTA::Region* region);//New

private:
	bool to_remove;
    void updateScoutAssignments();
	MicroManager * microManager;
	Regions* regions;
};
#endif 