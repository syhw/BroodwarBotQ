#include <PrecompiledHeader.h>
#include "Micro/Goals/ExploreGoal.h"

using namespace BWAPI;
using namespace std;
	
void ExploreGoal::needAScoutingUnit()
{	
	if (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Observer) > 0)
		_neededUnits.insert(make_pair<UnitType, int>(UnitTypes::Protoss_Observer, 1));
	else
		_neededUnits.insert(make_pair<UnitType, int>(UnitTypes::Protoss_Probe, 1));
}

ExploreGoal::ExploreGoal(TilePosition tp, int priority) 
: Goal(priority)
{
	if (Broodwar->isVisible(tp))
	{
		_status = GS_ACHIEVED;
		return;
	}
	needAScoutingUnit();
	_subgoals.push_back(pSubgoal(new SeeSubgoal(SL_AND, Position(tp))));
}

ExploreGoal::ExploreGoal(BWTA::Region* region, int priority) 
: Goal(priority)
{
	if (region != NULL)
	{
		needAScoutingUnit();

		BWTA::Polygon polygon = region->getPolygon();
		std::list<Position> to_see;
		bool insert=true;
		for(std::vector<Position>::iterator it = polygon.begin(); it != polygon.end(); ++it){
				to_see.push_back(*it);
		}
		
		//Add a first position to the subgoals
		Position prevPos = to_see.front();
		to_see.pop_front();
		_subgoals.push_back(pSubgoal(new SeeSubgoal(SL_AND, prevPos)));

		Position selectedPos;
		int size = to_see.size();
		double curDist;
		double maxDist;
		std::list<Position>::iterator it;
		while(!to_see.empty()){
			maxDist=0;

			//Select the furthest point
			for(it= to_see.begin(); it!= to_see.end(); ++it){
				curDist = it->getDistance(prevPos);
				if (curDist > maxDist){
					maxDist = curDist;
					selectedPos = (*it);
				}
			}

			//Create and push the associated Subgoal
			_subgoals.push_back(pSubgoal(new SeeSubgoal(SL_AND, selectedPos)));
			prevPos = selectedPos;

			//Remove this position from to_see
			to_see.remove(selectedPos);
			size --;
		}		
	}
}

void ExploreGoal::achieve()
{
	if (_status != GS_IN_PROGRESS) // defensive
		return;
	_unitsGroup.switchMode(MODE_SCOUT);
	check();
	/// Realize the subgoals IN ORDER (going though the Region)
	for(std::list<pSubgoal>::iterator it = _subgoals.begin(); it != _subgoals.end(); ++it){
		if (!((*it)->isRealized()))
			(*it)->tryToRealize();
	}
	_unitsGroup.update();
}

