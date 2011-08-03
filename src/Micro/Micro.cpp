#include <PrecompiledHeader.h>
#include "Micro/Micro.h"
#include "Micro/Goals/AvoidNukeGoal.h"

using namespace BWAPI;
using namespace std;

Micro::Micro()
: goalManager(NULL)
{
	goalManager = & GoalManager::Instance();
}

Micro::~Micro()
{
	GoalManager::Destroy();
}

void Micro::update()
{
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