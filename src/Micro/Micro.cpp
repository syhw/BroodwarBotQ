#include <PrecompiledHeader.h>
#include "Micro/Micro.h"
#include "Macro/InformationManager.h"
#include "Micro/Goals/AvoidNukeGoal.h"
#include "Micro/Goals/AttackGoal.h"
#include "Micro/Goals/DefendGoal.h"
#include "Micro/Goals/DropGoal.h"
#include "Micro/Goals/FormationSubgoal.h"
#include "Micro/Goals/KillSubgoal.h"
#include "Macro/BasesManager.h"
#include "Macro/BorderManager.h"
#include "Intelligence/EUnitsFilter.h"
#include "Intelligence/Intelligence.h"
#include "Defines.h"

using namespace BWAPI;
using namespace std;

Micro::Micro()
: goalManager(NULL)
, _launchedFirstPush(false)
, drops(0)
{
	goalManager = & GoalManager::Instance();
}

Micro::~Micro()
{
	GoalManager::Destroy();
}

void Micro::checkDefense()
{
	/// Seek for bases needing defense
	map<BWTA::Region*, BWTA::BaseLocation*> myBasesRegions;
	// go through all our bases and register them in myBasesRegions
	for each (Base* b in TheBasesManager->getAllBases())
	{
		if (needDefense.find(b->getBaseLocation()) == needDefense.end())
			myBasesRegions.insert(pair<BWTA::Region*, BWTA::BaseLocation*>(
				b->getBaseLocation()->getRegion(), b->getBaseLocation()));
	}

	map<BWTA::BaseLocation*, Vec> tmpMeanDefPositions;
	map<BWTA::BaseLocation*, int> tmpNumUnits;
	// go through all the EViewedUnits and find the ones in our regions
	for (map<Unit*, EViewedUnit>::const_iterator it 
		= EUnitsFilter::Instance().getViewedUnits().begin();
		it != EUnitsFilter::Instance().getViewedUnits().end(); ++it)
	{
		Position tmpUnitPos = it->second.position;
		if (it->first->isVisible())
			tmpUnitPos = it->first->getPosition();

		map<BWTA::Region*, BWTA::BaseLocation*>::const_iterator regit 
			= myBasesRegions.find(BWTA::getRegion(tmpUnitPos));
		if (regit != myBasesRegions.end())
		{
			if ((!(it->first->getType().isWorker()) 
					&& it->first->getType() != UnitTypes::Protoss_Observer)
				|| (it->first->isAttacking() || it->first->isConstructing()))
			{
				needDefense.insert(regit->second);
				if (tmpMeanDefPositions.find(regit->second)
					!= tmpMeanDefPositions.end())
				{
					tmpMeanDefPositions[regit->second] += Vec(it->first->getPosition());
					tmpNumUnits[regit->second] += 1;
				}
				else
				{
					tmpMeanDefPositions.insert(pair<BWTA::BaseLocation*,
						Vec>(regit->second, Vec(it->first->getPosition())));
					tmpNumUnits.insert(pair<BWTA::BaseLocation*,
						int>(regit->second, 1));
				}
			}
		}
	}

	// go through all the bases we have to defend and spawn the DefendGoal(s)
	for (map<BWTA::BaseLocation*, Vec>::const_iterator it
		= tmpMeanDefPositions.begin(); it != tmpMeanDefPositions.end(); ++it)
	{
		Vec tmpVecPos = Vec(it->second / tmpNumUnits[it->first]);
		pGoal tmp = pGoal(new DefendGoal(tmpVecPos.toPosition()));
		// OR pGoal tmp = new DefendGoal(it->first);
		goalManager->addGoal(tmp);
	}
}

void Micro::launchFirstPush()
{
	if ((Broodwar->enemy()->getRace() == Races::Protoss && (
			(Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Dragoon) > 1 
				&& ETechEstimator::Instance().getOpeningsProbas()[1] > 0.15 // fast DT
			|| TheInformationManager->getEnemyBases().size() > 1 ) // fast expand
			|| (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Dragoon) > 3))
		) 
		|| Broodwar->enemy()->getRace() == Races::Terran && (
			(Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Dragoon) > 1 
				&& ETechEstimator::Instance().getOpeningsProbas()[3] > 0.2 // siege expand
			|| TheInformationManager->getEnemyBases().size() > 1) // fast expand
			|| (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Dragoon) > 2) 
		) 
		|| Broodwar->enemy()->getRace() == Races::Zerg && (
			(Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Zealot) > 3 &&
			(ETechEstimator::Instance().getOpeningsProbas()[0] > 0.2 
				|| ETechEstimator::Instance().getOpeningsProbas()[1] > 0.2 // mutas
			|| TheInformationManager->getEnemyBases().size() > 1)) // fast expand
			|| Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Dragoon) > 2)
		)
	{
		Position p(Positions::None);
		p = (*(TheInformationManager->getEnemyBases().begin()))->getPosition();
		if (p != Positions::None)
		{
			pGoal tmp;
			/*if (Intelligence::Instance().enemyBasesOrder.size() > 1)
			{
				map<double, BWTA::BaseLocation*>::const_iterator it 
					= Intelligence::Instance().enemyBasesOrder.begin();
				++it;
				tmp = pGoal(new AttackGoal(it->second->getPosition(), 30));
			}
			else*/
			{
				tmp = pGoal(new AttackGoal(p, 30));
			}
			goalManager->addGoal(tmp);
			_launchedFirstPush = true;
		}
	}
}

void Micro::launchFirstDrop()
{
	BWTA::BaseLocation* eBase 
		= *(TheInformationManager->getEnemyBases().begin());
	if (eBase != NULL)
	{
		map<UnitType, int> tmpNeeded;
		tmpNeeded.insert(make_pair<UnitType, int>(UnitTypes::Protoss_Reaver, 1));
		/*Vec tmpV(0, 0);
		for each (Unit* mineral in eBase->getMinerals())
			tmpV += mineral->getPosition();
		BWAPI::Position mineralsMiddle = Position((int)tmpV.x / eBase->getMinerals().size(), (int)tmpV.y / eBase->getMinerals().size());
		goalManager->addGoal(pGoal(new DropGoal(mineralsMiddle, tmpNeeded)));*/
		goalManager->addGoal(pGoal(
			new DropGoal(eBase->getPosition(), tmpNeeded)));
	}
}

void Micro::update()
{
	/// spawn DefendGoal(s) if necessary
	checkDefense(); 

	/// Launch the first push as soon as we have a few dragoons and we know where the enemy is
	if (!_launchedFirstPush && needDefense.empty()
		&& !TheInformationManager->getEnemyBases().empty())
		launchFirstPush();

	/// Launch the first drop as soon as we have 1 shuttle and 1 reaver
	if (_launchedFirstPush && !drops
		&& Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Reaver)
		&& Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Shuttle)
		&& !TheInformationManager->getEnemyBases().empty())
		launchFirstDrop();

	/// Look if we can push (again)
	/*if (needDefense.empty() && _launchedFirstPush 
		&& (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Observer)
		|| Broodwar->self()->completedUnitCount(UnitTypes::Protoss_High_Templar)))
	{
		double theirForce = 0.0;
		for (map<Unit*, EViewedUnit>::const_iterator it = EUnitsFilter::Instance().getViewedUnits().begin();
			it != EUnitsFilter::Instance().getViewedUnits().end(); ++it)
		{
			if (!it->second.type.isBuilding()
				continue
		}
		if (ourForce > 1.5*theirForce)
			goalManager->
	}*/

	if (_launchedFirstPush && needDefense.empty() && goalManager->attackGoals < 1)
	{
		if (TheInformationManager->getEnemyBases().size() > TheBasesManager->getAllBases().size())
		{
			bool found = false;
			for (map<double, BWTA::BaseLocation*>::const_reverse_iterator it = Intelligence::Instance().enemyBasesOrder.rbegin();
				it != Intelligence::Instance().enemyBasesOrder.rend(); ++it)
			{
				if (TheInformationManager->getEnemyBases().count(it->second))
				{
					pGoal tmp = pGoal(new AttackGoal(it->second->getPosition()));
					BWAPI::Unit* cc = EUnitsFilter::Instance().getClosestCenter(it->second);
					if (cc != NULL)
						tmp->addSubgoal(pSubgoal(new KillSubgoal(SL_AND, NULL, cc)));
					goalManager->addGoal(tmp);
					found = true;
					break;
				}
			}
		}
		else
		{
			// pushing on his front choke
			Position toPush = Positions::None;
			for each (TilePosition tp in MapManager::Instance().getPathFromHomeToSL(Intelligence::Instance().enemyHome))
			{
				for each (BWTA::Chokepoint* cp in TheBorderManager->getEnemyBorder())
				{
					if (cp->getCenter().getApproxDistance(Position(tp)) <= 7*TILE_SIZE)
					{
						toPush = Position(tp);
						break;
					}
				}
			}
			if (toPush != Positions::None)
				goalManager->addGoal(pGoal(new AttackGoal(toPush)));
			else
				goalManager->addGoal(pGoal(new AttackGoal(Intelligence::Instance().enemyHome->getPosition())));
		}
	}

	/// Failsafe
	if (Broodwar->getFrameCount() > 8*24*60)
		_launchedFirstPush = true;

	/// Update all Goals
	goalManager->update();
}

void Micro::onUnitCreate(BWAPI::Unit *unit)
{
	goalManager->onUnitCreate(unit);

	/*if (unit->getPlayer() == Broodwar->self())
	{
		Race er = Broodwar->enemy()->getRace();
		UnitType ut = unit->getType();
		if (ut == UnitTypes::Protoss_High_Templar)
		{
			goalManager->addGoal(pGoal(new AttackGoal
		}
	}*/
}

void Micro::onUnitShow(BWAPI::Unit *unit)
{
	/// Spawn an intercepting DefendGoal
	if (unit->getPlayer() == Broodwar->enemy())
	{
		for each (Base* bb in TheBasesManager->getAllBases())
		{
			BWTA::BaseLocation* b = bb->getBaseLocation();
			if (Micro::Instance().needDefense.count(b))
				continue;
			BWAPI::Position bp = b->getPosition();
			if (unit->getDistance(bp) < __TILES_RADIUS_DEFEND_BASE__*TILE_SIZE
				|| (unit->getTargetPosition() != Positions::None 
					&& unit->getTargetPosition() != Positions::Invalid
				    && unit->getTargetPosition() != Positions::Unknown 
					&& unit->getTargetPosition().getApproxDistance(bp) 
						< __TILES_RADIUS_DEFEND_BASE__*TILE_SIZE*0.75) // !!
				|| (unit->getTarget() != NULL 
					&& unit->getTarget()->exists() 
					&& unit->getTarget()->getType().isBuilding() 
					&& unit->getTarget()->getPlayer() == Broodwar->self()))
			{
				GoalManager::Instance().addGoal(pGoal(new DefendGoal(b))); // TODO priority w.r.t. importance of the base
				Micro::Instance().needDefense.insert(b);
			}
		}
	}
}

void Micro::onUnitDestroy(BWAPI::Unit *unit)
{
	goalManager->onUnitDestroy(unit);
}

void Micro::onUnitRenegade(BWAPI::Unit *unit)
{
	goalManager->onUnitDestroy(unit); // to test TODO
}

void Micro::onNukeDetect(BWAPI::Position target)
{
	goalManager->addGoal(pGoal(new AvoidNukeGoal(target)));
}

BWAPI::Position Micro::getDefensePosition() const
{
#ifndef __MICRO_PROJECT__
	if (TheBasesManager->getAllBases().size() > 1)
		return TheBasesManager->getAllBases().back()->getBaseLocation()->getPosition();
	else
#endif
		return frontChoke->getCenter();
}