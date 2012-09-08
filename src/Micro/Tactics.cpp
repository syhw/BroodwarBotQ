#include <PrecompiledHeader.h>
#include "Micro/Tactics.h"
#include "Macro/BasesManager.h"
#include "Macro/InformationManager.h"
#include "Intelligence/EUnitsFilter.h"
#include "Intelligence/Intelligence.h"
#include "Defines.h"

using namespace BWAPI;
using namespace std;

double distClosestRegion(BWTA::Region* r, const set<BWTA::Region*>& s)
{
	double min = DBL_MAX;
	for each (BWTA::Region* r2 in s)
	{
		double dist = MapManager::Instance().distRegions(r, r2);
		if (dist < min)
			min = dist;
	}
	return min;
}

void Tactics::predictTheirAttack()
{
	set<BWTA::Region*> myRegions;
	for each (Base* b in TheBasesManager->getAllBases())
		myRegions.insert(b->getBaseLocation()->getRegion());
	set<BWTA::Region*> theirRegions;
	for each (pair<Unit*, EViewedUnit> evu in EUnitsFilter::Instance().getViewedUnits())
	{
		if (evu.second.type.isResourceDepot())
			theirRegions.insert(BWTA::getRegion(evu.second.position));
	}
	std::map<BWTA::Region*, std::vector<double> > belongs;
	belongs.swap(getBelongs(myRegions, theirRegions));
}

map<BWTA::Region*, vector<double> > getBelongs(const set<BWTA::Region*>& d_regs, const set<BWTA::Region*>& a_regs)
{
	std::map<BWTA::Region*, vector<double> > ret;
	/// This method say how much regions belong to the defender (d_regs)
	std::vector<double> mine;
	mine.push_back(0.0);
	mine.push_back(1.0);
	std::vector<double> their;
	their.push_back(1.0);
	their.push_back(0.0);
	vector<double> contested(2, 0.5);
	for each (BWTA::Region* r in BWTA::getRegions())
	{
		if (d_regs.count(r))
			ret.insert(make_pair<BWTA::Region*, vector<double> >(
				r, mine));
		else if (a_regs.count(r))
			ret.insert(make_pair<BWTA::Region*, vector<double> >(
				r, their));
		else
		{
			double d_d = distClosestRegion(r, d_regs);
			double d_a = distClosestRegion(r, a_regs);
			if (d_d <= 0.0 && d_a <= 0.0)
				ret.insert(make_pair<BWTA::Region*, vector<double> >(
					r, contested));
			else if (d_d < d_a)
			{
				vector<double> tmp = contested;
				tmp[0] *= d_d / d_a;
				tmp[1] = 1.0 - 0.5*(d_d/d_a);
				ret.insert(make_pair<BWTA::Region*, vector<double> >(
					r, tmp));
			}
			else
			{
				vector<double> tmp = contested;
				tmp[0] = 1.0 - 0.5*(d_a/d_d);
				tmp[1] *= (d_a/d_d);
				ret.insert(make_pair<BWTA::Region*, vector<double> >(
					r, tmp));
			}
		}
	}
	return ret;
}

map<BWTA::Region*, vector<double> > getTacticals(Player* p)
{
	std::map<BWTA::Region*, vector<double> > ret;
	return ret;
}

double Tactics::getAttackRegion(BWTA::Region* r)
{
	return attack_regions[r];
}

const std::vector<double>& Tactics::getHowRegion(BWTA::Region* r)
{
	return how_regions[r];
}