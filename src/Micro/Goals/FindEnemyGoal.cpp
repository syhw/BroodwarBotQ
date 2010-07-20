#include "FindEnemyGoal.h"

FindEnemyGoal::FindEnemyGoal(){
	scoutManager = & ScoutManager::Instance();
	pSubgoal sb;

	//Scout the different possible bases
	BWTA::BaseLocation * myStartLocation = BWTA::getStartLocation(BWAPI::Broodwar->self());
	std::set<BWTA::BaseLocation*> path = BWTA::getStartLocations();
		
	for(std::set<BWTA::BaseLocation*>::iterator p=path.begin();p!=path.end();p++){
		if( (*p) != myStartLocation){
			sb=pSubgoal(new SeeSubgoal(SL_AND, (*p)->getPosition()));
			addSubgoal(sb);
		}
	}
	sb=pSubgoal(new FindSubgoal(SL_OR));
	addSubgoal(sb);
}