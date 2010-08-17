#include "FindEnemyGoal.h"
#include "float.h"

FindEnemyGoal::FindEnemyGoal(){
	scoutManager = & ScoutManager::Instance();
	microManager = & MicroManager::Instance();
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

void FindEnemyGoal::achieve(){
	if(this->status != GS_NOT_ATTRIBUTED){
		checkAchievement();
			
		if(this->status!=GS_ACHIEVED){
			double test;
			pSubgoal selected;
			double min = DBL_MAX;
			
			for(std::list<pSubgoal>::iterator it = subgoals.begin(); it != subgoals.end(); ++it){
				if (!(*it)->isRealized()){
					test = (*it)->distanceToRealize();
					if ( test >= 0 && test < min ){
						min = test;
						selected = (*it);
					}
				}
			}
			if(min > 0 && min < DBL_MAX){
				selected->tryToRealize();

			}else{
				//TODO
				BWAPI::Broodwar->printf("Tell Louis this situation happened");
			}

		}else{
			//The goal is achieved, deleting the unitgroup
			microManager->promptRemove(unitsGroup);
		}
	}
}