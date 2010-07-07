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
#include "GoalManager.h"
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

	Regions* regions;
	//TODO positionsToSurvey; // to refresh infos about enemy bases: lord(zerg), scan/vessel?(terran), obs(toss)
	void onUnitCreate(BWAPI::Unit* unit);//New 
	void onUnitShow(BWAPI::Unit* unit);
	void findEnemy();//New
	std::list<BWTA::BaseLocation*> getBestPath( std::set<BWTA::BaseLocation* > baseLocations, BWTA::BaseLocation* myStartLocation) const;//New
	void exploreRegion(BWTA::Region* region);//New
	int newGoal() const ;//New   Return the number of new goals
	pGoal getGoal();//New Return the first element in the goal list

private:
	std::list<pGoal> scoutGoals; //New
	BWTA::BaseLocation* myStartLocation;//New
	GoalManager* goalManager;//New

    void updateScoutAssignments();
};
#endif 