#include <PrecompiledHeader.h>
#include "Micro/Micro.h"
#include "Macro/InformationManager.h"
#include "Micro/Goals/AvoidNukeGoal.h"
#include "Micro/Goals/AttackGoal.h"
#include "Micro/Goals/DefendGoal.h"
#include "Micro/Goals/DropGoal.h"
#include "Micro/Goals/FormationSubgoal.h"
#include "Macro/BasesManager.h"
#include "Macro/BorderManager.h"
#include "Intelligence/EUnitsFilter.h"
#include "Intelligence/Intelligence.h"
#include "Defines.h"

using namespace BWAPI;
using namespace std;

Micro::Micro()
: goalManager(NULL)
, _launchedFirstPush(false)
, drops(0)
{
	goalManager = & GoalManager::Instance();
}

Micro::~Micro()
{
	GoalManager::Destroy();
}

void Micro::update()
{
	/// Launch the first push as soon as we have 2 dragoons and we know where the enemy is
	if (!_launchedFirstPush && needDefense.empty()
		&& !TheInformationManager->getEnemyBases().empty()
		&& Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Dragoon) > 1)
	{
		Position p(Positions::None);
		p = (*(TheInformationManager->getEnemyBases().begin()))->getPosition();
		if (p != Positions::None)
		{
			pGoal tmp = pGoal(new AttackGoal(p, 60));
			if (Intelligence::Instance().enemyBasesOrder.size() > 1)
			{
				list<BWTA::BaseLocation*>::const_iterator it = Intelligence::Instance().enemyBasesOrder.begin();
				++it;
				tmp->addSubgoal(pSubgoal(new FormationSubgoal(SL_AND, NULL, 
					pFormation(new SquareFormation((*it)->getPosition())))));
			}
			goalManager->addGoal(tmp);
			_launchedFirstPush = true;
		}
	}

	/// Launch the first drop as soon as we have 1 shuttle and 1 reaver
	if (_launchedFirstPush && !drops
		&& Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Reaver)
		&& Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Shuttle)
		&& !TheInformationManager->getEnemyBases().empty())
	{
		map<UnitType, int> tmpNeeded;
		tmpNeeded.insert(make_pair<UnitType, int>(UnitTypes::Protoss_Reaver, 1));
		goalManager->addGoal(pGoal(new DropGoal(*(TheInformationManager->getEnemyBases().begin()), tmpNeeded)));
	}

	/// See if we have to defend somewhere
	// TODO CHANGE this monstruosity (use EUnitsFilter + units position filtering)
	/* TODO
	for each (BWTA::Chokepoint* c in TheBorderManager->getMyBorder())
	{
		bool needD = false;
		set<Unit*> unitsAround = Broodwar->getUnitsInRadius(b->getPosition(), __TILES_RADIUS_DEFEND_BASE__*TILE_SIZE);
		for each (Unit* u in unitsAround)
		{
			if (u->getPlayer() == Broodwar->enemy())
			{
				needD = true;
				break;
			}
		}
		if (needD) // we do not visit bases we have already spawned a DefendGoal on
		{
			if (needDefense.count(b))
				continue; // already spawned a DefendGoal
			goalManager->addGoal(pGoal(new DefendGoal(b))); // TODO priority w.r.t. importance of the base
			needDefense.insert(b);
		}
		else
			needDefense.erase(b);
	}*/
	for each (Base* bb in TheBasesManager->getAllBases())
	{
		BWTA::BaseLocation* b = bb->getBaseLocation();
		bool needD = false;
		set<Unit*> unitsAround = Broodwar->getUnitsInRadius(b->getPosition(), __TILES_RADIUS_DEFEND_BASE__*TILE_SIZE);
		for each (Unit* u in unitsAround)
		{
			if (u->getPlayer() == Broodwar->enemy())
			{
				needD = true;
				break;
			}
		}
		if (needD) // we do not visit bases we have already spawned a DefendGoal on
		{
			if (needDefense.count(b))
				continue; // already spawned a DefendGoal
			goalManager->addGoal(pGoal(new DefendGoal(b))); // TODO priority w.r.t. importance of the base
			needDefense.insert(b);
		}
		else
			needDefense.erase(b);
	}
	// TODO: for each building -> if is under attack

	/// Look if we can push (again)
	/*if (needDefense.empty() && _launchedFirstPush 
		&& (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Observer)
		|| Broodwar->self()->completedUnitCount(UnitTypes::Protoss_High_Templar)))
	{
		double theirForce = 0.0;
		for (map<Unit*, EViewedUnit>::const_iterator it = EUnitsFilter::Instance().getViewedUnits().begin();
			it != EUnitsFilter::Instance().getViewedUnits().end(); ++it)
		{
			if (!it->second.type.isBuilding()
				continue
		}
		if (ourForce > 1.5*theirForce)
			goalManager->
	}*/

	if (needDefense.empty() && goalManager->attackGoals < 1)
	{
		if (TheInformationManager->getEnemyBases().size() > TheBasesManager->getAllBases().size())
		{
			bool found = false;
			for (list<BWTA::BaseLocation*>::const_reverse_iterator it = Intelligence::Instance().enemyBasesOrder.rbegin();
				it != Intelligence::Instance().enemyBasesOrder.rend(); ++it)
			{
				if (TheInformationManager->getEnemyBases().count(*it))
				{
					goalManager->addGoal(pGoal(new AttackGoal((*it)->getPosition())));
					found = true;
					break;
				}
			}
			/*if (!found) 
			{
				for each (BWTA::Region* r in BWTA::getRegions())
					goalManager->addGoal(pGoal(new ExploreGoal(r)));
			}*/
		}
	}

	goalManager->update();
}

void Micro::onUnitCreate(BWAPI::Unit *unit)
{
	goalManager->onUnitCreate(unit);

	/*if (unit->getPlayer() == Broodwar->self())
	{
		Race er = Broodwar->enemy()->getRace();
		UnitType ut = unit->getType();
		if (ut == UnitTypes::Protoss_High_Templar)
		{
			goalManager->addGoal(pGoal(new AttackGoal
		}
	}*/
}

void Micro::onUnitShow(BWAPI::Unit *unit)
{
}

void Micro::onUnitDestroy(BWAPI::Unit *unit)
{
	goalManager->onUnitDestroy(unit);
}

void Micro::onUnitRenegade(BWAPI::Unit *unit)
{
	goalManager->onUnitDestroy(unit); // to test
}

void Micro::onNukeDetect(BWAPI::Position target)
{
	goalManager->addGoal(pGoal(new AvoidNukeGoal(target)));
}

BWAPI::Position Micro::getDefensePosition() const
{
	if (TheBasesManager->getAllBases().size() > 1)
		return TheBasesManager->getAllBases().back()->getBaseLocation()->getPosition();
	else
		return frontChoke->getCenter();
}