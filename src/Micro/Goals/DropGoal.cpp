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
	for each (pair<UnitType, int> pp in _neededUnits)
		bidOnUnitType(pp.first);
}

DropGoal::DropGoal(Position p, const map<UnitType, int>& nU, int priority)
: Goal(nU, priority)
, _dropPos(p)
{
	Micro::Instance().drops += 1;
	_neededUnits.insert(make_pair<UnitType, int>(UnitTypes::Protoss_Shuttle, 1));
	_base = BWTA::getNearestBaseLocation(_dropPos);
	for each (pair<UnitType, int> pp in _neededUnits)
		bidOnUnitType(pp.first);
}

DropGoal::~DropGoal()
{
	Micro::Instance().drops -= 1;
}

void DropGoal::achieve()
{
	Unit* reaver = NULL;
	for each (pBayesianUnit bu in _unitsGroup.units)
	{
		if (bu->getType() == UnitTypes::Protoss_Shuttle)
			_dropShipBu = bu;
		else
			_dropeeBu.push_back(bu);
		if (bu->getType() == UnitTypes::Protoss_Reaver)
			reaver = bu->unit;
	}
	if (_dropShipBu == NULL || _dropShipBu.get() == NULL 
		|| _dropShipBu->unit == NULL || !_dropShipBu->unit->exists() 
		|| _dropShipBu->getType() != UnitTypes::Protoss_Shuttle
		|| _dropeeBu.empty())
	{
		_status = GS_WAIT_PRECONDITION;
		return;
	}
	if (_dropShipBu->unit->getHitPoints() < 27)
	{
		_dropShipBu->unit->unloadAll();
		return;
	}
	if (_unitsGroup.getDistance(_dropPos) > 8*TILE_SIZE)
	{
		/// we go there
		if (_dropShipBu->unit->getLoadedUnits().size() == _unitsGroup.units.size() - 1) // only one dropship
		{
			/// gogogo
			_unitsGroup.nonFlyers = 0;
			_unitsGroup.groupTargetPosition = _dropPos;
			if (_unitsGroup.groupMode != MODE_SCOUT)
				_unitsGroup.switchMode(MODE_SCOUT);
			_unitsGroup.update();
			if (!(Broodwar->getFrameCount() % 25))
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
			/*if (reaver != NULL && !(Broodwar->getFrameCount() % 13))
			{
				reaver->train(UnitTypes::Protoss_Scarab);
				_dropShipBu->unit->load(reaver);
			}*/
			for each (pBayesianUnit pbu in _dropeeBu)
			{
				if (pbu->getMode() != MODE_FIGHT_G)
				{
					pbu->switchMode(MODE_FIGHT_G);
					pbu->update();
				}
			}
		}
	}
	else
	{
		/// dropship
		if (_dropShipBu->unit->getLoadedUnits().size())
			_dropShipBu->unit->unloadAll();
		else if (reaver != NULL 
			&& reaver->getGroundWeaponCooldown() > 0 
			&& !reaver->isStartingAttack())
			_dropShipBu->unit->load(reaver);

		/// others
		for each (pBayesianUnit pbu in _dropeeBu)
		{
			if (pbu->getMode() != MODE_FIGHT_G)
			{
				pbu->switchMode(MODE_FIGHT_G);
				pbu->update();
			}
		}
	}


}

void DropGoal::onOffer(set<Unit*> objects)
{
	GoalManager* gm = & GoalManager::Instance();
	//if ((_status == GS_WAIT_PRECONDITION || _status == GS_IN_PROGRESS)
	//	&& _unitsGroup.emptyUnits())
	if (_status == GS_WAIT_PRECONDITION)
	{
        for each (Unit* u in objects)
		{
			pBayesianUnit tmp = gm->getCompletedUnit(u);
			if (tmp
				&& _neededUnits.find(u->getType()) != _neededUnits.end() // take uniquely needed units
				&& _neededUnits[u->getType()] > 0)
			{
				TheArbitrator->accept(this, u, _priority);
				_neededUnits[u->getType()] -= 1;
				_unitsGroup.units.push_back(tmp);
			}
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

void DropGoal::update()
{
	Goal::update();
}