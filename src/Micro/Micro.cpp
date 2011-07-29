#include <PrecompiledHeader.h>
#include "Micro/Micro.h"

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
}

void Micro::onUnitShow(BWAPI::Unit *unit)
{
}

void Micro::onUnitDestroy(BWAPI::Unit *unit)
{
}