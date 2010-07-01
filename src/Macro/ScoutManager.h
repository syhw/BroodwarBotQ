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
#include "GoalManager.h"

class GoalManager;



class ScoutManager : public Arbitrator::Controller<BWAPI::Unit*,double>, public CSingleton<ScoutManager>
{
	friend class CSingleton<ScoutManager>;

	private:
		ScoutManager();
		~ScoutManager();

  public:
    class ScoutData
    {
      public:
        enum ScoutMode
        {
          Idle,
          Searching,
          Roaming,
          Harassing,
          Fleeing
        };
        ScoutData(){ mode = Idle; }
        BWAPI::Position target;
        ScoutMode mode;
    };


    virtual void onOffer(std::set<BWAPI::Unit*> units);
    virtual void onRevoke(BWAPI::Unit* unit, double bid);
    virtual void update();

    virtual std::string getName() const;
    void onUnitDestroy(BWAPI::Unit* unit);

    // Non-Controller methods.
    bool isScouting() const;
    void setScoutCount(int count);

		// Goals // Just ideas, not yet implemented
		void scoutAllEnemies();
		void counterWall();
		void counterBuild();
		void harassWorkers();
		void checkEmptyXP();

    Arbitrator::Arbitrator<BWAPI::Unit*,double>* arbitrator;
		Regions* regions;
   
    std::map<BWAPI::Unit*, ScoutData> scouts;
		//TODO positionsToSurvey; // to refresh infos about enemy bases: lord(zerg), scan/vessel?(terran), obs(toss)
    std::list<BWAPI::Position> positionsToScout;    

	void onUnitShow(BWAPI::Unit* unit);//New
	void onUnitCreate(BWAPI::Unit* unit);//New 
	void findEnemy();//New
	bool enemyFound() const;//New
	void setEnemyFound(bool b);//New
	std::list<BWTA::BaseLocation*> getBestPath( std::set<BWTA::BaseLocation* > baseLocations, BWTA::BaseLocation* myStartLocation) const;//New
	void exploreRegion(BWTA::Region* region);//New
	int newGoal() const ;//New   Return the number of new goals
	pGoal getGoal();//New Return the first element in the goal list
private:
	
	std::list<pGoal> scoutGoals; //New
	std::list<pGoal> assignedScoutGoals; //New
	BWTA::BaseLocation* myStartLocation;//New
	BWTA::BaseLocation* eStartLocation;//New
	bool bEnemyFound;//New
	GoalManager* goalManager;


    bool needMoreScouts() const;
    void requestScout(double bid);
    void addScout(BWAPI::Unit* unit);
    void updateScoutAssignments();

    size_t desiredScoutCount;
    int scoutingStartFrame;

		//TODO prendre en compte les positions allies pour explo a plusieur et reduire le tps a tous
		
		//std::pair<std::list<BWTA::BaseLocation*>, double> getBestPath( std::set<BWTA::BaseLocation* > baseLocations) const; //old getBestPath
		void showPath();
};
#endif 