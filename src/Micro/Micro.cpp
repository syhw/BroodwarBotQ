#include <PrecompiledHeader.h>
#include "Micro/Micro.h"
#include "Micro/Goals/AvoidNukeGoal.h"

using namespace BWAPI;

Micro::Micro()
{
	goalManager = & GoalManager::Instance();
}

Micro::~Micro()
{
}

void Micro::update()
{
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
	goalManager->onUnitRenegade(unit);
}

void Micro::onNukeDetect(BWAPI::Position target)
{
	goalManager->addGoal(pGoal(new AvoidNukeGoal(target)));
}