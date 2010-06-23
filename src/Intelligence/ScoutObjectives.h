#pragma once
#include <stdlib.h>
#include <BWAPI.h>
#include <BWTA.h>
#include "CSingleton.h"
class ScoutObjectives : public CSingleton<ScoutObjectives>
{
	friend class CSingleton<ScoutObjectives>;
	


public:
	std::list<BWAPI::Position> get_Objectives();
	void find_ennemy();
	std::list<BWTA::BaseLocation*> getBestPath( std::set<BWTA::BaseLocation* > baseLocations, BWTA::BaseLocation* myStartLocation) const;
	bool ennemy_found() const;
	void set_ennemy_found(bool b);
	void onUnitShow(BWAPI::Unit* unit);
	bool got_objectives();
	void explore_region(BWTA::Region* region);//add to the objectives the exploration of the region
	void accomplished(BWAPI::Position p);
private:
	ScoutObjectives();
	~ScoutObjectives();
	bool bEnnemy_found;
	std::list<BWAPI::Position> objectives;
	std::list<BWAPI::Position> assigned_objectives;
	BWTA::BaseLocation* myStartLocation;
	BWTA::BaseLocation* eStartLocation;
	
};