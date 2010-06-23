#include "ScoutObjectives.h"
#include <iostream>
using namespace BWAPI;


ScoutObjectives::ScoutObjectives():bEnnemy_found(false)
{

}

ScoutObjectives::~ScoutObjectives(){

}

bool ScoutObjectives::ennemy_found() const{
 return bEnnemy_found;
}

void ScoutObjectives::set_ennemy_found(bool b){
	bEnnemy_found=b;
}

std::list<Position> ScoutObjectives::get_Objectives(){
std::list<Position> temp;
this->objectives.swap(temp);
return temp;
}

void ScoutObjectives::find_ennemy(){

	//Scout the different possible bases
	myStartLocation = BWTA::getStartLocation(BWAPI::Broodwar->self());
	std::list<BWTA::BaseLocation*> path = getBestPath( BWTA::getStartLocations(), myStartLocation);
	Broodwar->printf("Number of bases : %d",path.size());

	for(std::list<BWTA::BaseLocation*>::iterator p=path.begin();p!=path.end();p++){
		objectives.push_back((*p)->getPosition());
	}
}


std::list<BWTA::BaseLocation*> ScoutObjectives::getBestPath( std::set<BWTA::BaseLocation* > baseLocations, BWTA::BaseLocation* myStartLocation) const
{
	std::list<BWTA::BaseLocation*> res;
	baseLocations.erase( myStartLocation);
	BWTA::BaseLocation* baseFrom = myStartLocation;
	while( !baseLocations.empty())
	{
		BWTA::BaseLocation* nearestBase = NULL;
		double dist = 0xFFFF;
		for( std::set<BWTA::BaseLocation* >::iterator b = baseLocations.begin(); b != baseLocations.end(); b++)
		{
			double dist2 = baseFrom->getGroundDistance( *b);
			if( dist2 <= 0) // Unreachable by walk.
				baseLocations.erase( nearestBase);
			if( dist2 < dist)
			{
				nearestBase = *b;
				dist = dist2;
			}
		}
		baseLocations.erase( nearestBase);
		res.push_back( nearestBase);
		baseFrom = nearestBase;
	}
	return res;
}

void ScoutObjectives::onUnitShow(BWAPI::Unit* unit){

	if(unit->getType().isResourceDepot() && ! this->ennemy_found()){
		Broodwar->printf("Test 1");
		this->set_ennemy_found(true);
		//Find the right base location
		std::list<BWTA::BaseLocation*> res;
		Broodwar->printf("Test 2");
		for(std::list<BWTA::BaseLocation* >::iterator b = res.begin(); b!= res.end(); b++){
			//Maybe try not to run all the BaseLocation possible
			if( (*b)->getTilePosition() == unit->getTilePosition() )
				eStartLocation = (*b);
		}
		Broodwar->printf("Test 3");
		if(eStartLocation == NULL)
			Broodwar -> printf("eStartLocation is NULL, problem...");
		Broodwar->printf("Ennemy main base found, waiting for objectives");
		Broodwar->printf("Test 4");
		//scout_all_EBase();
	}
		//We assume that if the ennemy was not spotted and we find a ResourceDepot, then it is the main base : requires fast scout
}

void ScoutObjectives::scout_all_EBase(){
	Broodwar->printf("Test 5");
	Broodwar->printf("Must explore the base");
	//Go to minerals
	Broodwar->printf("Test 6");

	std::set<BWAPI::Unit*> minerals = eStartLocation->getMinerals();
	for(std::set<BWAPI::Unit*>::const_iterator it=minerals.begin(); it!=minerals.end(); it++){
		objectives.push_back((*it)->getPosition());
	}
	Broodwar->printf("Test 7");
	Broodwar->printf("Got %d new objectives", objectives.size());
}


bool ScoutObjectives::got_objectives(){

	if(objectives.size() > 0)
		return true;

	return false;
}
