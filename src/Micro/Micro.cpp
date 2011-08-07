#include <PrecompiledHeader.h>
#include "Micro/Micro.h"
#include "Macro/InformationManager.h"
#include "Micro/Goals/AvoidNukeGoal.h"
#include "Micro/Goals/AttackGoal.h"
#include "Micro/Goals/DefendGoal.h"
#include "Micro/Goals/DropGoal.h"
#include "Macro/BasesManager.h"
#include "Macro/BorderManager.h"
#include "Intelligence/EUnitsFilter.h"
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
			goalManager->addGoal(pGoal(new AttackGoal(p, 60)));
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
	for each (Base* bb in TheBasesManager->getAllBases())
	{
		BWTA::BaseLocation* b = bb->getBaseLocation();
		bool needD = false;
		int eUnits = 0;
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

	goalManager->update();
}

void Micro::onUnitCreate(BWAPI::Unit *unit)
{
	goalManager->onUnitCreate(unit);
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