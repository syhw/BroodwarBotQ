#include <PrecompiledHeader.h>
#include "Intelligence/Intelligence.h"
#include "Macro/InformationManager.h" // temporary, or is it?
#include "Macro/Producer.h"
#include "Macro/BasesManager.h"
#include "Micro/Goals/Goal.h"
#include "Micro/Goals/FirstScoutGoal.h"
#include "Micro/Goals/ExploreGoal.h"
#include "BWTA.h"
#include "Macro/Macro.h"

using namespace BWAPI;

#define __POOL_TIME_RUSH__ 133 // seconds, 3 workers + 1 pool + 13 seconds
#define __BBS_TIME_RUSH__ 230 // seconds, 4 workers + 2 barracks + 18 seconds
#define __GATES_TIME_RUSH__ 190 // seconds, 4 workers + 2 gateways + 18 seconds

Intelligence::Intelligence()
: _launchedFirstScoutGoal(false)
, _launchedFirstExploreGoal(false)
, enemyRush(false)
, enemyRace(Races::Unknown)
, enemyHome(NULL)
, closestOnPath(-1)
{
	eUnitsFilter = & EUnitsFilter::Instance();
#ifdef __ETECH_ESTIMATOR__
	eTechEstimator = & ETechEstimator::Instance();
#endif
	mapManager = & MapManager::Instance();

	/// This is not so dumb: for when BWAPI getRace() will work with Random
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
#ifdef __ETECHESTIMATOR_DEBUG__
	eTechEstimator->onFrame();
#endif
#endif

#ifndef __MICRO_PROJECT__
	if (Broodwar->enemy()->getRace() == Races::Protoss)
	{
		if (Broodwar->getFrameCount() < 24*__GATES_TIME_RUSH__ + 12
			&& (eUnitsFilter->getNumbersType(UnitTypes::Protoss_Gateway) >= 2
			|| eUnitsFilter->getNumbersType(UnitTypes::Protoss_Photon_Cannon) >= 1))
		{
			enemyRush = true;
			TheProducer->produce(2, UnitTypes::Protoss_Zealot, 100);
		}
	}
	else if (Broodwar->enemy()->getRace() == Races::Terran)
	{
		if (Broodwar->getFrameCount() < 24*__BBS_TIME_RUSH__
			&& eUnitsFilter->getNumbersType(UnitTypes::Terran_Barracks) >= 2)
		{
			enemyRush = true;
			TheProducer->produce(3, UnitTypes::Protoss_Dragoon, 100);
			Macro::Instance().reaverFirst = true;
		}
	}
	else if (Broodwar->enemy()->getRace() == Races::Zerg)
	{
		if (Broodwar->getFrameCount() < 24*__POOL_TIME_RUSH__ + 12
			&& eUnitsFilter->getNumbersType(UnitTypes::Zerg_Spawning_Pool))
		{
			enemyRush = true;
			TheProducer->produce(2, UnitTypes::Protoss_Zealot, 100);
			//if (!TheBasesManager->getAllBases().empty())
			//	TheBasesManager->getHomeBase()->setActiveGas(false);
		}
	}

	if (Broodwar->getFrameCount() > 6*60*24)
		enemyRush = false;
#ifdef __INTELLIGENCE_DEBUG__
	if (enemyRush)
		Broodwar->drawTextScreen(585, 18, "\x08RUSH");
	else
		Broodwar->drawTextScreen(585, 18, "\x07safe");
#endif

	/// Initialize the enemy's bases order if we know where he is
	/// (_enemyBasesOrder should never be empty again, or else there is something really wrong)
	if (_enemyBasesOrder.empty() && TheInformationManager->getEnemyBases().size())
	{
		for each (BWTA::BaseLocation* b in TheInformationManager->getEnemyBases()) // in case he expanded first
		{
			if (BWTA::getStartLocations().count(b))
			{
				enemyHome = b;
				enemyBasesOrder.insert(std::make_pair<double, BWTA::BaseLocation*>(0.0, b));
				break;
			}
		}
		if (enemyHome == NULL)
			enemyHome = *(TheInformationManager->getEnemyBases().begin());

		for each (BWTA::BaseLocation* b in BWTA::getBaseLocations())
		{
			if (b != enemyHome)
			{
				double dist = MapManager::Instance().distBaseToBase(enemyHome, b);
				if (dist > 0.0)
					enemyBasesOrder.insert(std::make_pair<double, BWTA::BaseLocation*>(dist, b));
				else
					enemyBasesOrder.insert(std::make_pair<double, BWTA::BaseLocation*>(DBL_MAX, b));			
			}
		}
		for each (std::pair<double, BWTA::BaseLocation*> it in enemyBasesOrder)
		{
			_enemyBasesOrder.push_back(it.second);
		}
	}

	/*if (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Observer) > currentlyExploring.size() + 2
		&& !scouting)
	{
		ExploreGoal(list enemy bases) TODO
		scouting = true;
	}*/

	/// Scout bases if we think they have a hidden one
	if (!_enemyBasesOrder.empty()
		&& Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Observer) > (int)(currentlyExploring.size() + 2) // leave at least 3 observers for the armies 
		&& TheInformationManager->getEnemyBases().size() <= TheBasesManager->getAllBases().size())
	{
		if (TheInformationManager->getEnemyBases().count(_enemyBasesOrder.front())
		    || TheBasesManager->getRegionsBases().count(_enemyBasesOrder.front()->getRegion())
			|| currentlyExploring.count(_enemyBasesOrder.front()->getRegion()))
		{
			_enemyBasesOrder.push_back(_enemyBasesOrder.front());
			_enemyBasesOrder.pop_front();
		}
		else
		{
			GoalManager::Instance().addGoal(pGoal(new ExploreGoal(_enemyBasesOrder.front()->getRegion())));
			currentlyExploring.insert(_enemyBasesOrder.front()->getRegion());
			_launchedFirstExploreGoal = true;
			// push_back(front) + pop_front done one frame later by above code
		}
	}
#ifdef __INTELLIGENCE_DEBUG__
	int cc = 0;
	for each (BWTA::BaseLocation* b in _enemyBasesOrder)
	{
		Broodwar->drawCircleMap(b->getPosition().x(), b->getPosition().y(), 16, Colors::Orange);
		Broodwar->drawTextMap(b->getPosition().x() - 8, b->getPosition().y() - 8, "%d", cc++);
	}
#endif

	if (!_launchedFirstExploreGoal && enemyRush == false && enemyHome != NULL 
		&& ETechEstimator::Instance().hasInfered
		&& Broodwar->getFrameCount() > 2500 && maxVector(ETechEstimator::Instance().getOpeningsProbas()) < 0.28)
	{
		GoalManager::Instance().addGoal(pGoal(new ExploreGoal(enemyHome->getRegion())));
		_launchedFirstExploreGoal = true;
	}
#endif
	if (Broodwar->getFrameCount() > 20*24*60 && EUnitsFilter::Instance().empty()) // failsafe to explore all
	{
		for each (BWTA::Region* r in BWTA::getRegions())
		{
			GoalManager::Instance().addGoal(pGoal(new ExploreGoal(r)));
		}
	}
}

void Intelligence::onUnitCreate(Unit* u)
{
	mapManager->onUnitCreate(u);
#ifndef __MICRO_PROJECT__
	if (!_launchedFirstScoutGoal 
		&& u->getPlayer() == Broodwar->self() 
		&& u->getType() == Broodwar->self()->getRace().getSupplyProvider()) // pylon scout
	{
		pGoal tmp(new FirstScoutGoal(90));
		GoalManager::Instance().addGoal(tmp);
		_launchedFirstScoutGoal = true;
	}
#endif
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