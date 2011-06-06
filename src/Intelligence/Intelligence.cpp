#include <PrecompiledHeader.h>
#include "Intelligence/Intelligence.h"

using namespace BWAPI;

Intelligence::Intelligence()
{
	eUnitsFilter = & EUnitsFilter::Instance();
	eTechEstimator = & ETechEstimator::Instance();
	mapManager = & MapManager::Instance();
	scoutController = & ScoutController::Instance();
}

Intelligence::~Intelligence()
{
    ETechEstimator::Destroy();
    EUnitsFilter::Destroy();
    MapManager::Destroy();
	ScoutController::Destroy();
}

void Intelligence::update()
{
	eUnitsFilter->update();
	mapManager->update();
	scoutController->update();
}

void Intelligence::onUnitCreate(Unit* u)
{
	mapManager->onUnitCreate(u);
	scoutController->onUnitCreate(u);
}

void Intelligence::onUnitDestroy(Unit* u)
{
	eUnitsFilter->onUnitDestroy(u);
	eTechEstimator->onUnitDestroy(u);
	mapManager->onUnitDestroy(u);
	scoutController->onUnitDestroy(u);
}

void Intelligence::onUnitShow(Unit* u)
{
	eUnitsFilter->onUnitShow(u);
	eTechEstimator->onUnitShow(u);
	mapManager->onUnitShow(u);
	scoutController->onUnitShow(u);
}

void Intelligence::onUnitHide(Unit* u)
{
	eUnitsFilter->onUnitHide(u);
	eTechEstimator->onUnitHide(u);
	mapManager->onUnitHide(u);
}

void Intelligence::onUnitMorph(Unit* u)
{
	eUnitsFilter->onUnitMorph(u);
}

void Intelligence::onUnitRenegade(Unit* u)
{
	eUnitsFilter->onUnitRenegade(u);
}