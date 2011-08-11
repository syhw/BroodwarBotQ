#include <PrecompiledHeader.h>
#include "Micro/Goals/DropGoal.h"
#include "Micro/Micro.h"
#include "Subgoal.h"
#include "Macro/InformationManager.h"

using namespace BWAPI;
using namespace std;

/// TODO Refactor this: instead of not using the units group, do a
/// BasicUnitsGroup
/// DropUnitsGroup : BasicUnitsGroup
/// FightUnitsGroup : BasicUnitsGroup
/// and have the a BasicUnitsGroup* in Goal to runtime dispatch the good update()
/// instead of hacking in here (nonFlyers / switchMode(MODE_SCOUT)...)
DropGoal::DropGoal(BWTA::BaseLocation* b, const std::map<BWAPI::UnitType, int>& nU, int priority)
: Goal(nU, priority)
, _base(b)
{
	Micro::Instance().drops += 1;
	_neededUnits.insert(make_pair<UnitType, int>(UnitTypes::Protoss_Shuttle, 1));
	_dropPos = b->getPosition();
}

DropGoal::DropGoal(Position p, const map<UnitType, int>& nU, int priority)
: Goal(nU, priority)
, _dropPos(p)
{
	Micro::Instance().drops += 1;
	_neededUnits.insert(make_pair<UnitType, int>(UnitTypes::Protoss_Shuttle, 1));
	_base = BWTA::getNearestBaseLocation(_dropPos);
}

DropGoal::~DropGoal()
{
	Micro::Instance().drops -= 1;
}

void DropGoal::achieve()
{
	for each (pBayesianUnit bu in _unitsGroup.units)
	{
		if (bu->getType() == UnitTypes::Protoss_Shuttle)
			_dropShipBu = bu;
		if (bu->getType() == UnitTypes::Protoss_Reaver)
			_reaverBu = bu;
	}
	if (_dropShipBu == NULL || _dropShipBu.get() == NULL || _dropShipBu->unit == NULL || !_dropShipBu->unit->exists() || _dropShipBu->getType() != UnitTypes::Protoss_Shuttle
		|| _reaverBu == NULL || _reaverBu.get() == NULL || _reaverBu->unit == NULL || !_reaverBu->unit->exists() || _reaverBu->getType() != UnitTypes::Protoss_Reaver)
	{
		_status = GS_WAIT_PRECONDITION;
		return;
	}
	if (_dropShipBu->unit->getHitPoints() < 27)
	{
		_dropShipBu->unit->unload(_reaverBu->unit);
		return;
	}
	if (_unitsGroup.getDistance(_dropPos) > 8*TILE_SIZE)
	{
		/// we go there
		if (_dropShipBu->unit->getLoadedUnits().count(_reaverBu->unit))
		{
			/// gogogo
			_unitsGroup.nonFlyers = 0;
			if (_unitsGroup.groupMode != MODE_SCOUT)
				_unitsGroup.switchMode(MODE_SCOUT);
			_unitsGroup.update();
			if (!(Broodwar->getFrameCount() % 23))
			{
				if (_dropShipBu->getPPath().empty())
				{
					if (_dropShipBu->_fleeingDmg < 45)
					{
						_dropShipBu->_fleeingDmg += 10;
					}
					else
					{
						/// Drop elsewhere
						for each (BWTA::BaseLocation* b in TheInformationManager->getEnemyBases())
						{
							if (b != _base)
							{
								_base = b;
								_dropPos = b->getPosition();
								return;
							}
						}
					}
				}
			}
		}
		else
		{
			if (!(Broodwar->getFrameCount() % 13))
			{
				_reaverBu->unit->train(UnitTypes::Protoss_Scarab);
				_dropShipBu->unit->load(_reaverBu->unit);
			}
			_reaverBu->switchMode(MODE_FIGHT_G);
			_reaverBu->update();
		}
	}
	else
	{
		/// dropship
		if (_dropShipBu->unit->getLoadedUnits().count(_reaverBu->unit))
			_dropShipBu->unit->unload(_reaverBu->unit);
		else if (_reaverBu->unit->getGroundWeaponCooldown() > 0 && !_reaverBu->unit->isStartingAttack())
			_dropShipBu->unit->load(_reaverBu->unit);

		/// reaver
		if (_reaverBu->getMode() != MODE_FIGHT_G)
		{
			_reaverBu->switchMode(MODE_FIGHT_G);
			_reaverBu->update();
		}
	}


}

void DropGoal::onOffer(set<Unit*> objects)
{
	GoalManager* gm = & GoalManager::Instance();
	if ((_status == GS_WAIT_PRECONDITION || _status == GS_IN_PROGRESS)
		&& _unitsGroup.emptyUnits())
	{
		Position choke = (*BWTA::getStartLocation(Broodwar->self())->getRegion()->getChokepoints().begin())->getCenter();
		double minDist = DBL_MAX;
		Unit* acceptedUnit = NULL;
        for each (Unit* u in objects)
		{
			if (gm->getCompletedUnits().find(u) != gm->getCompletedUnits().end()
				&& _neededUnits.find(u->getType()) != _neededUnits.end() // take uniquely needed units
				&& _neededUnits[u->getType()] > 0)
			{
				if (u->getDistance(choke) < minDist)
				{
					acceptedUnit = u;
					minDist = u->getDistance(choke);
				}
			}
		}
		if (acceptedUnit != NULL)
		{
			TheArbitrator->accept(this, acceptedUnit, _priority);
			_neededUnits[acceptedUnit->getType()] -= 1;
			_unitsGroup.dispatchCompleteUnit(gm->getCompletedUnit(acceptedUnit));
		}
	}
	else
	{
		TheArbitrator->decline(this, objects, 0);
		TheArbitrator->removeBid(this, objects);
        for each (Unit* u in objects)
			_biddedOn.erase(u);
	}
}