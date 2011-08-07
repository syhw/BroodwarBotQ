#include <PrecompiledHeader.h>
#include "Micro/Goals/AttackGoal.h"
#include "Subgoal.h"
#include "FormationSubgoal.h"
#include "Micro/Formations.h"
#include "Macro/BWSAL.h"
#include "Macro/BorderManager.h"
#include "Micro/Goals/RegroupGoal.h"

using namespace BWAPI;
using namespace std;

AttackGoal::AttackGoal(const map<UnitType, int>& miniUnits, BWAPI::Position p,
					   int priority, int firstFrame)
: Goal(miniUnits,
	   priority,
	   firstFrame)
{
	addSubgoal(pSubgoal(new FormationSubgoal(SL_OR, &_unitsGroup,
	                                 pFormation(new SquareFormation(p)))));
}

AttackGoal::AttackGoal(BWAPI::Position p, int priority, int firstFrame)
: Goal(priority,
	   firstFrame)
{
    addSubgoal(pSubgoal(new FormationSubgoal(SL_OR, &_unitsGroup,
	                                 pFormation(new SquareFormation(p)))));
}

AttackGoal::AttackGoal(pSubgoal subgoal, int priority, int firstFrame)
: Goal(subgoal,
	   priority,
	   firstFrame)
{
}

void AttackGoal::achieve()
{
#ifdef __MID_SUBGOALS__
	if (_subgoals.size() < 10)
	{
		//See if we need an intermediate subgoal : 
		double min = 99999999.9;
		for(std::list<pSubgoal>::iterator it = _subgoals.begin(); it != _subgoals.end(); ++it){
			if ((*it)->distanceToRealize() > 0 && (*it)->distanceToRealize() < min)
	        {
				min = (*it)->distanceToRealize();
			}
		}
		if (min > 1337.0)
			createMidSubgoal();
	}
#endif
	bidOnMilitaryUnits();

	/// Cancel if we are getting pwned
	if (_unitsGroup.force < 0.75)
		abort();

	Goal::achieve();
}

#ifdef __MID_SUBGOALS__
void AttackGoal::createMidSubgoal()
{
    if (_unitsGroup.ppath.empty())
        return;
    Position tmpPos = _unitsGroup.ppath[_unitsGroup.ppath.size() / 2];
    BWTA::Region* r = BWTA::getRegion(TilePosition(tmpPos));
	tmpPos = MapManager::Instance().regionsPFCenters[r];
	//Create an intermediate subgoal at half the way of the path of the unitsgroup
    if (r != BWTA::getRegion(TilePosition(_unitsGroup.center)))
        addSubgoal(pSubgoal(new FormationSubgoal(SL_AND, &_unitsGroup, pFormation(new SquareFormation(tmpPos)))));
}
#endif

std::string AttackGoal::getName()
{
	return "AttackGoal";
}

std::string AttackGoal::getShortName()
{
	return "AttG";
}

/// RegroupGoal in the middle of the map
void AttackGoal::abort()
{
	TilePosition mid(Broodwar->mapWidth()/2, Broodwar->mapHeight()/2);
	if (!Broodwar->isWalkable(mid.x() * 4, mid.y() * 4))
		mid = MapManager::Instance().closestWalkabableSameRegionOrConnected(mid);
	if (BWTA::getRegion(mid)->isReachable(BWTA::getRegion(TilePosition(_unitsGroup.center))))
		GoalManager::Instance().addGoal(pGoal(new RegroupGoal(Position(mid))));
	else
		GoalManager::Instance().addGoal(pGoal(new RegroupGoal(BWTA::getStartLocation(Broodwar->self())->getPosition())));
	_status = GS_ACHIEVED;
}