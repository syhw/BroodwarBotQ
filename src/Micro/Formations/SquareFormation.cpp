#include <PrecompiledHeader.h>
#include "SquareFormation.h"
#include "Regions/MapManager.h"

//#define __SAFE_SQUARE_FORMATION__

using namespace BWAPI;

SquareFormation::SquareFormation(const SquareFormation& f)
: Formation(f) {}

SquareFormation::SquareFormation(const Vec& center, const Vec& direction)
: Formation(center, direction) {}

SquareFormation::SquareFormation(const Position& p, const Vec& direction)
: Formation(p, direction) {}

void SquareFormation::computeToPositions(const std::vector<pBayesianUnit>& vUnits)
{
	computeToPositions(vUnits, 0);
}

void SquareFormation::computeToPositions(const std::vector<pBayesianUnit>& vUnits, unsigned int unwantedSlots)
{
    if (!vUnits.size())
        return;
	end_positions.clear();

#ifdef __SAFE_SQUARE_FORMATION__
	BWTA::Region* regionCenter = BWTA::getRegion(TilePosition(center.toPosition()));
	if (regionCenter != NULL)
		center = MapManager::Instance().regionsPFCenter(regionCenter);
#endif
	if (!Broodwar->isWalkable(center.toPosition().x()/8, center.toPosition().y()/8))
		center = Vec(MapManager::Instance().closestWalkabableSameRegionOrConnected(center.toPosition()));

	if (unwantedSlots >= vUnits.size())
	{
		for (unsigned int i = 0; i < vUnits.size(); ++i)
			end_positions.push_back(Position((int)center.x, (int)center.y));
		return;
	}

    if (vUnits.size() == 1)
    {
        end_positions.push_back(Position((int)center.x, (int)center.y));
        return;
    }

    // find how many contact units we will have, and store their indices
    /*** TODO BUG HERE TODO TODO TODO
    std::set<unsigned int> contactUnits;
	for (unsigned int i = 0; i < vUnits.size(); i++)
    {
        Broodwar->printf("id %d %s", vUnits[i]->getType().getID(), vUnits[i]->getType().getName().c_str());
        //if (vUnits[i]->getType().getID() == 66 || vUnits[i]->getType().getID() == 68)
        if (vUnits[i]->getType() == BWAPI::UnitTypes::Protoss_Zealot 
            || vUnits[i]->getType() == BWAPI::UnitTypes::Protoss_Archon)
        {
            contactUnits.insert(i);
        }
    }*/

    int maxDim = 0;
    for (std::vector<pBayesianUnit>::const_iterator it = vUnits.begin();
        it != vUnits.end(); ++it)
    {
        if ((*it)->getMaxDimension() > maxDim)
            maxDim = (*it)->getMaxDimension();
    }
	int sizeSide = int(sqrt((double)(vUnits.size() + unwantedSlots)) + 1);
	int maxContainance = sizeSide*sizeSide;
	unsigned int moreSlots = 0;
	Vec corner = center - Position( (int)(sizeSide/2.0 * (space + maxDim)), (int)(sizeSide/2.0 * (space + maxDim)));
	//goto safe_exit;
	unsigned int ii = 0;
	for (unsigned int i = 0; i < (unsigned int)maxContainance && ii < vUnits.size(); ++i)
    {
        Position topos = (corner + Position(int(i/sizeSide) * (space + maxDim), (i%sizeSide) * (space + maxDim))).toPosition();
        if (vUnits[ii]->unit->getType().isFlyer())
        {
			end_positions.push_back(topos);
			++ii;
		}
		bool isWalkable = Broodwar->isWalkable(topos.x()/8, topos.y()/8);
		bool isGoodHeight = (Broodwar->getGroundHeight(TilePosition(topos)) == Broodwar->getGroundHeight(TilePosition(center.toPosition())));
		if (!isWalkable || !isGoodHeight)
		{
			++moreSlots;
			continue;
		}
        if (!topos.isValid())
            topos.makeValid();
		if (end_positions.size() < vUnits.size())
		{
			end_positions.push_back(topos);
			++ii;
		}
		else
			break;
    }
    computeMean();
    if (vUnits.size() != end_positions.size())
    {
        end_positions.clear();
        this->computeToPositions(vUnits, unwantedSlots + moreSlots);
    }

    /*** TODO BUG HERE TODO TODO TODO
    // add contact units in the direction of the formation, in the outer layer/ring if no direction
    if (!contactUnits.empty() && !contactUnits.size() == vUnits.size())
    {
        unsigned int nContactUnits = contactUnits.size();
        Vec tmpDir = Vec(-direction.x, -direction.y);
        tmpDir *= sizeSide;
        Position opposite = tmpDir.translate(center.toPosition());
        std::list<unsigned int> placedUnits;
        std::map<double, unsigned int> slots;
        // find/generate slots
        for (unsigned int i = 0; i < vUnits.size(); i++)
        {
            double dist = - end_positions[i].getDistance(opposite);
            if (dist < slots.begin()->first)
            {
                if (slots.size() >= nContactUnits)
                    slots.erase(slots.begin());
                slots.insert(std::make_pair<double, unsigned int>(dist, i));
            }
        }
        // prune already in place contact units
        for (std::map<double, unsigned int>::const_iterator it = slots.begin();
            it != slots.end(); )
        {
            if (vUnits[it->second]->getType() == UnitTypes::Protoss_Zealot
                || vUnits[it->second]->getType() == UnitTypes::Protoss_Archon)
            {
                contactUnits.erase(it->second);
                slots.erase(it++);
            }
            else
                ++it;
        }
        // place the other contact units not in place yet
        for (std::map<double, unsigned int>::const_iterator it = slots.begin();
            it != slots.end(); ++it)
        {
            Position tmpPos = end_positions[it->second];
            end_positions[it->second] = end_positions[*(contactUnits.begin())];
            end_positions[*(contactUnits.begin())] = tmpPos;
            contactUnits.erase(contactUnits.begin());
        }
    }
    */
	return;
/*safe_exit:
	{
		Broodwar->printf("ERROR IN SQUAREFORMATION");
		log("ERROR IN SQUAREFORMATION");
		for (unsigned int i = 0; i < vUnits.size(); ++i)
			end_positions.push_back(center.toPosition());
		return;
	}*/
}
