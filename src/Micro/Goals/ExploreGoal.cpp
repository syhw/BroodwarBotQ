#include <PrecompiledHeader.h>
#include "Micro/Goals/ExploreGoal.h"
#include "Intelligence/Intelligence.h"
#include "Macro/Arbitrator.h"
#include "Macro/BWSAL.h"

using namespace BWAPI;
using namespace std;
	
void ExploreGoal::needAScoutingUnit()
{	
	if (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Observer) > 1)
		_neededUnits.insert(make_pair<UnitType, int>(UnitTypes::Protoss_Observer, 1));
	else
		_neededUnits.insert(make_pair<UnitType, int>(UnitTypes::Protoss_Probe, 1));
}

ExploreGoal::ExploreGoal(BWTA::Region* region, int priority) 
: Goal(priority)
, _region(region)
, _firstRealized(0)
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
		_subgoals.push_back(pSubgoal(new SeeSubgoal(SL_AND, &_unitsGroup, prevPos)));

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
			_subgoals.push_back(pSubgoal(new SeeSubgoal(SL_AND, &_unitsGroup, selectedPos)));
			prevPos = selectedPos;

			//Remove this position from to_see
			to_see.remove(selectedPos);
			size--;
		}		
	}
	else
		_status = GS_ACHIEVED;
}

ExploreGoal::~ExploreGoal()
{
	Intelligence::Instance().currentlyExploring.erase(_region);
}

void ExploreGoal::achieve()
{
	if (_status != GS_IN_PROGRESS) // defensive
		return;
	if (Intelligence::Instance().enemyRush || (_firstRealized && Broodwar->getFrameCount() - _firstRealized > 1200)) // if we are taking more than 50 sec to explore a region, we're doing it wrong
	{
		_status = GS_ACHIEVED;
		return;
	}
	_unitsGroup.switchMode(MODE_SCOUT);
	check();
	/// Realize the subgoals IN ORDER (going though the Region)
	for(std::list<pSubgoal>::iterator it = _subgoals.begin(); it != _subgoals.end(); ++it)
	{
		if (!((*it)->isRealized()))
			(*it)->tryToRealize();
		else if (!_firstRealized) // to unblock/unstuck on bad regions geometry
			_firstRealized = Broodwar->getFrameCount();
	}
	_unitsGroup.update();
}


void ExploreGoal::onOffer(set<Unit*> objects)
{
	GoalManager* gm = & GoalManager::Instance();
	if (_status == GS_WAIT_PRECONDITION || _status == GS_IN_PROGRESS)
	{
        for each (Unit* u in objects)
		{
			if (_neededUnits.find(u->getType()) != _neededUnits.end()
				&& _neededUnits[u->getType()] > 0)
			{
				_neededUnits[u->getType()] -= 1;
				TheArbitrator->accept(this, u, _priority);
				if (gm->getCompletedUnits().find(u) != gm->getCompletedUnits().end())
				{
					_unitsGroup.dispatchCompleteUnit(gm->getCompletedUnit(u));
				}
				else
					_incompleteUnits.push_back(u);
			}
			else
			{
				TheArbitrator->decline(this, u, 0);
				TheArbitrator->removeBid(this, u);
				_biddedOn.erase(u);
			}
		}
	}
	else
	{
		TheArbitrator->decline(this, objects, 0);
		TheArbitrator->removeBid(this, objects);
        for each (Unit* u in objects)
			_biddedOn.erase(u);
	}
}