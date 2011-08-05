#include <PrecompiledHeader.h>
#include "Micro/Goals/AttackGoal.h"
#include "Subgoal.h"
#include "FormationSubgoal.h"
#include "Micro/Formations.h"
#include "Macro/BWSAL.h"

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
	//See if we need an intermediate subgoal : 
	double min = 99999999.9;
	for(std::list<pSubgoal>::iterator it = _subgoals.begin(); it != _subgoals.end(); ++it){
		if ((*it)->distanceToRealize() > 0 && (*it)->distanceToRealize() < min)
        {
			min = (*it)->distanceToRealize();
		}
	}
	if (min > 1337.0)
    {
		this->createMidSubgoal();
	}
	Goal::achieve();
}

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

std::string getName()
{
	return "AttackGoal";
}

void AttackGoal::canBidOn(Unit* u)
{
	bidOnUnit(u);
}

/*void AttackGoal::onOffer(set<Unit*> objects)
{
	GoalManager* gm = & GoalManager::Instance();
	if (_status == GS_WAIT_PRECONDITION || _status == GS_IN_PROGRESS)
	{
        for each (Unit* u in objects)
		{
			TheArbitrator->accept(this, u, _priority);
			if (_neededUnits.find(u->getType()) != _neededUnits.end())
				_neededUnits[u->getType()] -= 1;
			_unitsGroup.dispatchCompleteUnit(gm->getCompletedUnit(u));
			gm->unassignedUnits.erase(u);
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
}*/