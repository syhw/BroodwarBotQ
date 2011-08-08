#include <PrecompiledHeader.h>
#include "Micro/Goals/AttackGoal.h"
#include "Subgoal.h"
#include "FormationSubgoal.h"
#include "Micro/Formations.h"
#include "Macro/BWSAL.h"
#include "Macro/BorderManager.h"
#include "Micro/Goals/RegroupGoal.h"
#include "Intelligence/Intelligence.h"

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
	bidOnMilitaryUnits();
}

AttackGoal::AttackGoal(BWAPI::Position p, int priority, int firstFrame)
: Goal(priority,
	   firstFrame)
{
    addSubgoal(pSubgoal(new FormationSubgoal(SL_OR, &_unitsGroup,
	                                 pFormation(new SquareFormation(p)))));
	bidOnMilitaryUnits();
}

AttackGoal::AttackGoal(pSubgoal subgoal, int priority, int firstFrame)
: Goal(subgoal,
	   priority,
	   firstFrame)
{
	bidOnMilitaryUnits();
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

/// RegroupGoal at the closest, backing point on the path from home to enemy's
void AttackGoal::abort()
{
	TilePosition tp;
	BWTA::BaseLocation* eHome = Intelligence::Instance().enemyHome;
	if (BWTA::getRegion(TilePosition(_unitsGroup.center)) == eHome->getRegion())
	{
		std::list<BWTA::BaseLocation*>::const_iterator it = Intelligence::Instance().enemyBasesOrder.begin();
		++it;
		tp = (*it)->getTilePosition();
	}
	else
	{
		tp = MapManager::Instance().getPathFromHomeToSL(eHome)[2*MapManager::Instance().getPathFromHomeToSL(eHome).size()/3];
	}
	GoalManager::Instance().addGoal(pGoal(new RegroupGoal(Position(tp))));
	_status = GS_CANCELED;
}