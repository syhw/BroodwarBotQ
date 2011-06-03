#include <PrecompiledHeader.h>
#include "Micro.h"

using namespace BWAPI;

Micro::Micro()
{
	goalManager = & GoalManager::Instance();
	warManager = & WarManager::Instance();
}

Micro::~Micro()
{
}

void Micro::update()
{
	warManager->update();
}

void Micro::onUnitCreate(BWAPI::Unit *unit)
{
	warManager->onUnitCreate(unit);
}

void Micro::onUnitDestroy(BWAPI::Unit *unit)
{
	warManager->onUnitDestroy(unit);
}