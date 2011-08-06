#include <PrecompiledHeader.h>
#include "Micro/Micro.h"
#include "Macro/InformationManager.h"
#include "Micro/Goals/AvoidNukeGoal.h"
#include "Micro/Goals/AttackGoal.h"
#include "Macro/BasesManager.h"

using namespace BWAPI;
using namespace std;

Micro::Micro()
: goalManager(NULL)
, _launchedFirstPush(false)
{
	goalManager = & GoalManager::Instance();
}

Micro::~Micro()
{
	GoalManager::Destroy();
}

void Micro::update()
{
	if (!_launchedFirstPush
		&& !TheInformationManager->getEnemyBases().empty()
		&& Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Dragoon) > 1)
	{
		Position p(Positions::None);
		p = (*(TheInformationManager->getEnemyBases().begin()))->getPosition();
		if (p != Positions::None)
		{
			goalManager->addGoal(pGoal(new AttackGoal(p, 90)));
			_launchedFirstPush = true;
		}
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