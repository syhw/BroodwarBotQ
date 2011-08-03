#include <PrecompiledHeader.h>
#include "Intelligence/Intelligence.h"
#include "Macro/InformationManager.h" // temporary, or is it?
#include "Micro/Goals/Goal.h"
#include "Micro/Goals/FirstScoutGoal.h"

using namespace BWAPI;

#define __POOL_TIME_RUSH__ 130 // seconds, 3 workers + 1 pool + 11 seconds
#define __BBS_TIME_RUSH__ 230 // seconds, 4 workers + 2 barracks + 18 seconds
#define __GATES_TIME_RUSH__ 190 // seconds, 4 workers + 2 gateways + 18 seconds

Intelligence::Intelligence()
: _launchedFirstScoutGoal(false)
, enemyRush(false)
{
	eUnitsFilter = & EUnitsFilter::Instance();
	eTechEstimator = & ETechEstimator::Instance();
	mapManager = & MapManager::Instance();
}

Intelligence::~Intelligence()
{
    ETechEstimator::Destroy();
    EUnitsFilter::Destroy();
    MapManager::Destroy();
}

void Intelligence::update()
{
	eUnitsFilter->update();
	mapManager->update();
#ifdef __DEBUG__
	eTechEstimator->onFrame();
#endif
	if (Broodwar->enemy()->getRace() == Races::Protoss)
	{
		if (Broodwar->getFrameCount() < __GATES_TIME_RUSH__ + 20
			&& eUnitsFilter->getNumbersType(UnitTypes::Protoss_Gateway) >= 2)
				enemyRush = true;
	}
	else if (Broodwar->enemy()->getRace() == Races::Terran)
	{
		if (Broodwar->getFrameCount() < __BBS_TIME_RUSH__
			&& eUnitsFilter->getNumbersType(UnitTypes::Terran_Barracks) >= 2)
				enemyRush = true;
	}
	else if (Broodwar->enemy()->getRace() == Races::Zerg)
	{
		if (Broodwar->getFrameCount() < __POOL_TIME_RUSH__ + 20
			&& eUnitsFilter->getNumbersType(UnitTypes::Zerg_Spawning_Pool))
				enemyRush = true;
	}
#ifdef __DEBUG__
	if (enemyRush)
		Broodwar->drawTextScreen(585, 20, "RUSH");
	else
		Broodwar->drawTextScreen(585, 20, "safe");
#endif
}

void Intelligence::onUnitCreate(Unit* u)
{
	mapManager->onUnitCreate(u);
	if (!_launchedFirstScoutGoal 
		&& u->getPlayer() == Broodwar->self() 
		&& u->getType() == Broodwar->self()->getRace().getSupplyProvider())
	{
		pGoal tmp(new FirstScoutGoal(90));
		GoalManager::Instance().addGoal(tmp);
		_launchedFirstScoutGoal = true;
	}
}

void Intelligence::onUnitDestroy(Unit* u)
{
	eUnitsFilter->onUnitDestroy(u);
	eTechEstimator->onUnitDestroy(u);
	mapManager->onUnitDestroy(u);
}

void Intelligence::onUnitShow(Unit* u)
{
	eUnitsFilter->onUnitShow(u);
	eTechEstimator->onUnitShow(u);
	mapManager->onUnitShow(u);
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