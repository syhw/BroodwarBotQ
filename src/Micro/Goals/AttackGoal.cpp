#include <PrecompiledHeader.h>
#include "Micro/Goals/AttackGoal.h"
#include "Subgoal.h"
#include "FormationSubgoal.h"
#include "Micro/Formations.h"
#include "Macro/BWSAL.h"
#include "Macro/BorderManager.h"
#include "Micro/Goals/RegroupGoal.h"
#include "Intelligence/Intelligence.h"
#include "Macro/BasesManager.h"

using namespace BWAPI;
using namespace std;

int closestOnPath(BWAPI::Position p, const std::vector<BWAPI::TilePosition>& vtp)
{
	if (vtp.empty())
		return -1;
	int i = 0;
	int min = 0;
	double minV = DBL_MAX;
	for each (TilePosition tp in vtp)
	{
		if (p.getApproxDistance(Position(tp)) < minV)
		{
			min = i;
			minV = p.getApproxDistance(Position(tp));
		}
		++i;
	}
	return min;
}

AttackGoal::AttackGoal(const map<UnitType, int>& miniUnits, BWAPI::Position p,
					   int priority, int firstFrame)
: Goal(miniUnits,
	   priority,
	   firstFrame)
{
	addSubgoal(pSubgoal(new FormationSubgoal(SL_OR, &_unitsGroup,
	                                 pFormation(new SquareFormation(p)))));
	bidOnMilitaryUnits();
	GoalManager::Instance().attackGoals += 1;
}

AttackGoal::AttackGoal(BWAPI::Position p, int priority, int firstFrame)
: Goal(priority,
	   firstFrame)
{
    addSubgoal(pSubgoal(new FormationSubgoal(SL_OR, &_unitsGroup,
	                                 pFormation(new SquareFormation(p)))));
	bidOnMilitaryUnits();
	GoalManager::Instance().attackGoals += 1;
}

AttackGoal::AttackGoal(pSubgoal subgoal, int priority, int firstFrame)
: Goal(subgoal,
	   priority,
	   firstFrame)
{
	bidOnMilitaryUnits();
	GoalManager::Instance().attackGoals += 1;
}

AttackGoal::~AttackGoal()
{
	GoalManager::Instance().attackGoals -= 1;
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
	if (_unitsGroup.force < 0.75 && Broodwar->getFrameCount() - _firstActive > 10*24)
		abort();

	/// Regroup
	if (_unitsGroup.center.isValid() && Broodwar->isWalkable(_unitsGroup.center.x()/8, _unitsGroup.center.y()/8)
		&& _unitsGroup.stdDevRadius > (double)_unitsGroup.size() * TILE_SIZE / 2)
	{
		for (std::vector<pBayesianUnit>::iterator it = _unitsGroup.units.begin(); it != _unitsGroup.units.end(); ++it)
		{
#ifdef __DEBUG__
			Position displayp = (*it)->unit->getPosition();
			Broodwar->drawTextMap(displayp.x() + 8, displayp.y() + 8, "\x07 Regroup");
#endif
			(*it)->target = _unitsGroup.center;
			if ((*it)->getType().isFlyer())
				(*it)->switchMode(MODE_FIGHT_A);
			else
				(*it)->switchMode(MODE_FIGHT_G);
		}
	}
	else /// And go!
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
	// Shitload of hacks
	TilePosition tp;
	BWTA::BaseLocation* eHome = Intelligence::Instance().enemyHome;
	if (eHome == NULL)
		tp = BWTA::getStartLocation(Broodwar->self())->getTilePosition();
	else
	{
		int cOP = closestOnPath(_unitsGroup.center, MapManager::Instance().getPathFromHomeToSL(eHome));
		/*if (BWTA::getRegion(TilePosition(_unitsGroup.center)) == eHome->getRegion())
		{
			std::list<BWTA::BaseLocation*>::const_iterator it = Intelligence::Instance().enemyBasesOrder.begin();
			++it;
			tp = (*it)->getTilePosition();
		}
		else*/
		{
			if (cOP < 0 || cOP >= (int)MapManager::Instance().getPathFromHomeToSL(eHome).size())
			{
				if (!TheBasesManager->getAllBases().empty())
					tp = TheBasesManager->getAllBases().back()->getBaseLocation()->getTilePosition();
				else
					tp = BWTA::getStartLocation(Broodwar->self())->getTilePosition();
			}
			else
			{
				tp = MapManager::Instance().getPathFromHomeToSL(eHome)[3*cOP/4];
				Intelligence::Instance().closestOnPath = cOP;
			}
		}
	}
	GoalManager::Instance().addGoal(pGoal(new AttackGoal(Position(tp), _priority)));
	//GoalManager::Instance().addGoal(pGoal(new RegroupGoal(Position(tp))));
	_status = GS_CANCELED;
}