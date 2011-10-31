#include <PrecompiledHeader.h>
#include "Micro/Goals/ExploreGoal.h"

using namespace BWAPI;

ExploreGoal::ExploreGoal(TilePosition tp) 
: Goal()
{
	if (Broodwar->isVisible(tp))
	{
		status = GS_ACHIEVED;
		return;
	}
	subgoals.push_back(pSubgoal(new SeeSubgoal(SL_AND, Position(tp))));
}

ExploreGoal::ExploreGoal(BWTA::Region* region) 
: Goal()
{
	if (region != NULL)
	{
		BWTA::Polygon polygon = region->getPolygon();
		std::list<Position> to_see;
		bool insert=true;
		for(std::vector<Position>::iterator it = polygon.begin(); it != polygon.end(); ++it){
				to_see.push_back(*it);
		}
		
		//Add a first position to the subgoals
		Position prevPos = to_see.front();
		to_see.pop_front();
		subgoals.push_back(pSubgoal(new SeeSubgoal(SL_AND, prevPos)));

		
		Position selectedPos;
		int size = to_see.size();
		double curDist;
		double maxDist;
		std::list<Position>::iterator it;
		while(!to_see.empty()){
			maxDist=0;

			//Select the farest point
			for(it= to_see.begin(); it!= to_see.end(); ++it){
				curDist = it->getDistance(prevPos);
				if (curDist > maxDist){
					maxDist = curDist;
					selectedPos = (*it);
				}
			}


			//Create and push the associated Subgoal

			subgoals.push_back(pSubgoal(new SeeSubgoal(SL_AND, selectedPos)));
			prevPos = selectedPos;

			//Remove this position from to_see
			to_see.remove(selectedPos);
			size --;

		}		
	}
}

void ExploreGoal::achieve(){

	checkAchievement();
	// !!! Accomplish the subgoals in order
	if(status!=GS_ACHIEVED){
	
		for(std::list<pSubgoal>::iterator it = subgoals.begin(); it != subgoals.end(); ++it){
			if (!((*it)->isRealized()))
            {
				(*it)->tryToRealize();
			}
		}
	}
}

