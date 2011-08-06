#include <PrecompiledHeader.h>
#include "Intelligence/Intelligence.h"
#include "Macro/InformationManager.h" // temporary, or is it?
#include "Macro/Producer.h"
#include "Micro/Goals/Goal.h"
#include "Micro/Goals/FirstScoutGoal.h"

using namespace BWAPI;

#define __POOL_TIME_RUSH__ 130 // seconds, 3 workers + 1 pool + 11 seconds
#define __BBS_TIME_RUSH__ 230 // seconds, 4 workers + 2 barracks + 18 seconds
#define __GATES_TIME_RUSH__ 190 // seconds, 4 workers + 2 gateways + 18 seconds

Intelligence::Intelligence()
: _launchedFirstScoutGoal(false)
, enemyRush(false)
, enemyRace(Races::Unknown)
{
	eUnitsFilter = & EUnitsFilter::Instance();
#ifdef __ETECH_ESTIMATOR__
	eTechEstimator = & ETechEstimator::Instance();
#endif
	mapManager = & MapManager::Instance();

	/// This is not so dumb: for when Random will work
	if (Broodwar->enemy()->getRace() == Races::Protoss)
		enemyRace = Races::Protoss;
	else if (Broodwar->enemy()->getRace() == Races::Terran)
		enemyRace = Races::Terran;
	else if (Broodwar->enemy()->getRace() == Races::Zerg)
		enemyRace = Races::Zerg;
}

Intelligence::~Intelligence()
{
#ifdef __ETECH_ESTIMATOR__
    ETechEstimator::Destroy();
#endif
    EUnitsFilter::Destroy();
    MapManager::Destroy();
}

void Intelligence::update()
{
	eUnitsFilter->update();
	mapManager->update();
#ifdef __ETECH_ESTIMATOR__
#ifdef __DEBUG__
	eTechEstimator->onFrame();
#endif
#endif
	if (Broodwar->enemy()->getRace() == Races::Protoss)
	{
		if (Broodwar->getFrameCount() < 24*__GATES_TIME_RUSH__ + 12
			&& eUnitsFilter->getNumbersType(UnitTypes::Protoss_Gateway) >= 2)
		{
			enemyRush = true;
			TheProducer->produce(1, UnitTypes::Protoss_Zealot, 100);
		}
	}
	else if (Broodwar->enemy()->getRace() == Races::Terran)
	{
		if (Broodwar->getFrameCount() < 24*__BBS_TIME_RUSH__
			&& eUnitsFilter->getNumbersType(UnitTypes::Terran_Barracks) >= 2)
				enemyRush = true;
	}
	else if (Broodwar->enemy()->getRace() == Races::Zerg)
	{
		if (Broodwar->getFrameCount() < 24*__POOL_TIME_RUSH__ + 12
			&& eUnitsFilter->getNumbersType(UnitTypes::Zerg_Spawning_Pool))
		{
			enemyRush = true;
			TheProducer->produce(2, UnitTypes::Protoss_Zealot, 100);
		}
	}
	if (Broodwar->getFrameCount() > 10*60*24)
		enemyRush = false;
#ifdef __DEBUG__
	if (enemyRush)
		Broodwar->drawTextScreen(585, 18, "\x08RUSH");
	else
		Broodwar->drawTextScreen(585, 18, "\x07safe");
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
#ifdef __ETECH_ESTIMATOR__
	eTechEstimator->onUnitDestroy(u);
#endif
	mapManager->onUnitDestroy(u);
}

void Intelligence::onUnitShow(Unit* u)
{
	if (enemyRace == Races::Unknown && u->getPlayer() == Broodwar->enemy())
		enemyRace = u->getType().getRace(); // the first enemy unit we see have to be the enemy's race

	eUnitsFilter->onUnitShow(u);
#ifdef __ETECH_ESTIMATOR__
	eTechEstimator->onUnitShow(u);
#endif
	mapManager->onUnitShow(u);
}

void Intelligence::onUnitHide(Unit* u)
{
	eUnitsFilter->onUnitHide(u);
#ifdef __ETECH_ESTIMATOR__
	eTechEstimator->onUnitHide(u);
#endif
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