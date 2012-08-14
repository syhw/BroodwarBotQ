#include <PrecompiledHeader.h>
#include "Macro/Macro.h"
#include "Macro/Planner.h"
#include "Macro/ReservedMap.h"
#include "Macro/UnitGroupManager.h"
#include "Defines.h"

using namespace BWAPI;
using namespace std;

Planner::Planner()
{
	ThePlanner = this;
}

Planner::~Planner()
{
	ThePlanner = NULL;
}

void Planner::buildOrder(const BWAPI::UnitType& ut, int supplyAsTime)
{
	build_order.insert(make_pair<int, const BWAPI::UnitType&>(supplyAsTime, ut));
}

void Planner::insert_plan(const UnitType& ut, int prio)
{
	for (list<pl_unit>::const_iterator it = plan.begin();
		it != plan.end(); ++it)
	{
		if (prio > it->prio)
		{
			plan.insert(it, pl_unit(ut, prio));
			return;
		}
	}
}

void Planner::add(const UnitType& ut, int number, int prio, TilePosition tp)
{
	/// Guard for requests that we already have (buildings at a given pos)
	for (list<pl_unit>::iterator it = request.begin();
		it != requests.end(); ++it)
	{
		if (it->tilepos != TilePositions::None && 
			it->tilepos == tp && it->type == ut)
		{
			if (it->prio < prio)
				it->prio = prio;
			return;
		}
	}

	/// push_back as many requests as we need
	for (int i = 0; i < number; ++i)
		requests.push_back(pl_unit(ut, prio, tp));
}

void Planner::update()
{
	/// if not RUSH and not build_order.empty() just follow the BO
	/// else (RUSH or build_order.empty()) plan w.r.t. priorities

	/// Follow the BO
	if (!build_order.empty())
	{
		list<multimap<int, const UnitType&>::iterator> toRemove;
		for (multimap<int, const UnitType&>::iterator it = build_order.begin();
			it != build_order.end(); ++it)
		{
			if (it->first <= Broodwar->self()->supplyUsed()/2)
			{
#ifdef __MACRO_DEBUG__
				Broodwar->printf("Building %s pop %d", it->second->getName().c_str(), Broodwar->self()->supplyUsed()/2);
#endif
				insert_plan(it->second, 99);
				toRemove.push_back(it);
			}
		}
		for (list<multimap<int, const UnitType&>::iterator>::const_iterator
			it = toRemove.begin(); it != toRemove.end(); ++it)
			build_order.erase(*it);
	}

	/// Plan
}
