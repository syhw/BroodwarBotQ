#pragma once
#include <CSingleton.h>
#include <Arbitrator.h>
#include <BWAPI.h>
#include <Regions.h>
#include "ScoutObjectives.h"
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
    std::list<BWAPI::Position> positionsToScout;
		//TODO positionsToSurvey; // to refresh infos about enemy bases: lord(zerg), scan/vessel?(terran), obs(toss)
        
  private:
    ScoutObjectives* scoutObjectives;
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