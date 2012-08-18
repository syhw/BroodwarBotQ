#include <PrecompiledHeader.h>
#include "Micro/Goals/DropGoal.h"
#include "Micro/Micro.h"
#include "Subgoal.h"
#include "Macro/InformationManager.h"

using namespace BWAPI;
using namespace std;

Position interm_position(BWTA::BaseLocation* b)
{
	Vec tmp;
	for each (BWTA::Chokepoint* c in b->getRegion()->getChokepoints())
		tmp += Vec(c->getCenter().x() - b->getPosition().x(), c->getCenter().y() - b->getPosition().y());
	tmp = tmp.normalize();
	tmp *= - 12*TILE_SIZE;
	Position tmpPos = tmp.translate(Vec(b->getPosition()));
	if (tmpPos.x() >= Broodwar->mapWidth() * TILE_SIZE)
		tmpPos = Position((Broodwar->mapWidth()-1) * TILE_SIZE, tmpPos.y());
	if (tmpPos.y() >= Broodwar->mapHeight() * TILE_SIZE)
		tmpPos = Position(tmpPos.x(), (Broodwar->mapHeight()-2) * TILE_SIZE);
	if (tmpPos.x() <= 0)
		tmpPos = Position(TILE_SIZE, tmpPos.y());
	if (tmpPos.y() <= 0)
		tmpPos = Position(tmpPos.x(), TILE_SIZE);
	return tmpPos;
}

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
	_dropPos = Position(max(2*TILE_SIZE, b->getPosition().x() - 2*TILE_SIZE), b->getPosition().y());
	_intermPos = interm_position(b);
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
	_intermPos = interm_position(_base);
	for each (pair<UnitType, int> pp in _neededUnits)
		bidOnUnitType(pp.first);
}

DropGoal::~DropGoal()
{
	Micro::Instance().drops -= 1;
}

void DropGoal::achieve()
{
#ifdef __MICRO_DEBUG__
	Broodwar->drawCircleMap(_dropPos.x(), _dropPos.y(), 40, Colors::Purple, true);
	Broodwar->drawCircleMap(_intermPos.x(), _intermPos.y(), 40, Colors::Brown, true);
#endif
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
		_status = GS_CANCELED;
		return;
	}
	if (_dropShipBu->unit->getHitPoints() < 50)
	{
		if (!(Broodwar->getFrameCount() % (3+Broodwar->getLatencyFrames())))
			_dropShipBu->unit->unloadAll();
		return;
	}
	//if (_unitsGroup.getDistance(_dropPos) > 16*TILE_SIZE)
	if (_dropShipBu->unit->getDistance(_dropPos) > 14*TILE_SIZE)
	{
		/// we go there
		if (_dropShipBu->unit->getLoadedUnits().size() == _unitsGroup.units.size() - 1) // only one dropship
		{
			/// gogogo
			_unitsGroup.nonFlyers = 0;
			_unitsGroup.groupTargetPosition = _intermPos;

			/*if (_unitsGroup.groupMode != MODE_SCOUT)   TODO
				_unitsGroup.switchMode(MODE_SCOUT);      TODO
			_unitsGroup.update();                        TODO */
			_dropShipBu->unit->move(_intermPos);

			if (!(Broodwar->getFrameCount() % 25))
			{
				if (_dropShipBu->getPPath().empty())
				{
					if (_dropShipBu->_fleeingDmg < 65)
					{
						_dropShipBu->_fleeingDmg += 10;
					}
					/*else
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
					}*/
				}
			}
		}
		else /// load reaver
		{
			if (reaver != NULL && !(Broodwar->getFrameCount() % 13))
			{
				reaver->train(UnitTypes::Protoss_Scarab);
				if (_dropShipBu->unit->getHitPoints() > 42)
					_dropShipBu->unit->load(reaver);
			} 
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
		if (_dropShipBu->unit->getDistance(_dropPos) > 6*TILE_SIZE)
		{
			if (_dropShipBu->unit->getLoadedUnits().count(reaver))
			{
				if (!(Broodwar->getFrameCount() % 23))
					_dropShipBu->move(_dropPos);
			}
			else if (reaver != NULL && !(Broodwar->getFrameCount() % 14))
				_dropShipBu->unit->load(reaver);
		}
		else
		{
			if (!(Broodwar->getFrameCount() % 27))
			{
				/// dropship
				if (_dropShipBu->unit->getLoadedUnits().size())
					_dropShipBu->unit->unload(reaver);
					//_dropShipBu->unit->unloadAll();
				else if (reaver != NULL 
					&& reaver->getGroundWeaponCooldown() > 0 
					&& !reaver->isStartingAttack()
					&& _dropShipBu->unit->getHitPoints() > 42)
					_dropShipBu->unit->load(reaver);

			}
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