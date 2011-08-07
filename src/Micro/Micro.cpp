#include <PrecompiledHeader.h>
#include "Micro/Micro.h"
#include "Macro/InformationManager.h"
#include "Micro/Goals/AvoidNukeGoal.h"
#include "Micro/Goals/AttackGoal.h"
#include "Micro/Goals/DropGoal.h"
#include "Macro/BasesManager.h"

using namespace BWAPI;
using namespace std;

Micro::Micro()
: goalManager(NULL)
, _launchedFirstPush(false)
, _launchedFirstDrop(false)
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
	if (!_launchedFirstPush
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
	if (_launchedFirstPush && !_launchedFirstDrop
		&& Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Reaver)
		&& Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Shuttle)
		&& !TheInformationManager->getEnemyBases().empty())
	{
		map<UnitType, int> tmpNeeded;
		tmpNeeded.insert(make_pair<UnitType, int>(UnitTypes::Protoss_Reaver, 1));
		goalManager->addGoal(pGoal(new DropGoal(*(TheInformationManager->getEnemyBases().begin()), tmpNeeded)));
		_launchedFirstDrop = true;
	}

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