#include <PrecompiledHeader.h>
#include "Macro/SimCityBuildingPlacer.h"
#include "Macro/Heap.h"
#include "Utils/Vec.h"
#include "Regions/MapManager.h"
#include "Macro/BasesManager.h"
#include "Macro/Builder.h"
#include "Micro/Micro.h"
#include "Macro/InformationManager.h"

using namespace std;
using namespace BWAPI;

/// TODO: more consistent/frequent buildings power (pylons coverage) checks (see onUnitDestroy) and convexity/hasPath checks

/***
 * returns true if a (Manhattan, sure) path inside a square of size 
 * max(dist(tp1, tp2).x, dist(tp1, tp2).y) exists between tp1 and tp2
 */
inline bool existsInnerPath(const TilePosition& tp1,
							const TilePosition& tp2,
							const set<TilePosition>& occupied)
{
	if (tp1 == tp2) // same
		return true;
	if (abs(tp1.x()-tp2.x()) + abs(tp1.y()-tp2.y()) < 2) // adjacent in Manhattan distance
		return true;
	MapManager* mm = & MapManager::Instance();
	set<TilePosition> from1;
	from1.insert(tp1);
	set<TilePosition> from2;
	from2.insert(tp2);
	unsigned int max = max(abs(tp1.x() - tp2.x()), abs(tp1.y() - tp2.y()));
	for (unsigned int i = 0; i < max; ++i)
	{
		set<TilePosition> tmp1;
		for (set<TilePosition>::const_iterator it = from1.begin();
			it != from1.end(); ++it)
		{
			TilePosition tmp = TilePosition(it->x()+1, it->y());
			if (mm->isBTWalkable(tmp) && !occupied.count(tmp) && !TheReservedMap->isReserved(tmp))
				tmp1.insert(tmp);
			tmp = TilePosition(it->x(), it->y()+1);
			if (mm->isBTWalkable(tmp) && !occupied.count(tmp) && !TheReservedMap->isReserved(tmp))
				tmp1.insert(tmp);
			tmp = TilePosition(it->x()-1, it->y());
			if (mm->isBTWalkable(tmp) && !occupied.count(tmp) && !TheReservedMap->isReserved(tmp))
				tmp1.insert(tmp);
			tmp = TilePosition(it->x(), it->y()-1);
			if (mm->isBTWalkable(tmp) && !occupied.count(tmp) && !TheReservedMap->isReserved(tmp))
				tmp1.insert(tmp);
		}
		tmp1.swap(from1);
		set<TilePosition> tmp2;
		for (set<TilePosition>::const_iterator it = from2.begin();
			it != from2.end(); ++it)
		{
			TilePosition tmp = TilePosition(it->x()+1, it->y());
			if (from1.count(tmp))
				return true;
			if (mm->isBTWalkable(tmp) && !occupied.count(tmp) && !TheReservedMap->isReserved(tmp))
				tmp2.insert(tmp);
			tmp = TilePosition(it->x(), it->y()+1);
			if (from1.count(tmp))
				return true;
			if (mm->isBTWalkable(tmp) && !occupied.count(tmp) && !TheReservedMap->isReserved(tmp))
				tmp2.insert(tmp);
			tmp = TilePosition(it->x()-1, it->y());
			if (from1.count(tmp))
				return true;
			if (mm->isBTWalkable(tmp) && !occupied.count(tmp) && !TheReservedMap->isReserved(tmp))
				tmp2.insert(tmp);
			tmp = TilePosition(it->x(), it->y()-1);
			if (from1.count(tmp))
				return true;
			if (mm->isBTWalkable(tmp) && !occupied.count(tmp) && !TheReservedMap->isReserved(tmp))
				tmp2.insert(tmp);
		}
		tmp2.swap(from2);
	}
	return false;
}

list<TilePosition> manhattanWalkableNotReserved(const TilePosition& pos)
{
	list<TilePosition> ret;
	TilePosition tmp(pos.x()-1, pos.y());
	if (tmp.isValid() && MapManager::Instance().isBTWalkable(tmp) && !TheReservedMap->isReserved(tmp))
		ret.push_back(tmp);
	tmp = TilePosition(pos.x()+1, pos.y());
	if (tmp.isValid() && MapManager::Instance().isBTWalkable(tmp) && !TheReservedMap->isReserved(tmp))
		ret.push_back(tmp);
	tmp = TilePosition(pos.x(), pos.y()-1);
	if (tmp.isValid() && MapManager::Instance().isBTWalkable(tmp) && !TheReservedMap->isReserved(tmp))
		ret.push_back(tmp);
	tmp = TilePosition(pos.x(), pos.y()+1);
	if (tmp.isValid() && MapManager::Instance().isBTWalkable(tmp) && !TheReservedMap->isReserved(tmp))
		ret.push_back(tmp);
	return ret;
}

inline bool floodFillWalkable(const TilePosition& pos, const UnitType& ut, const TilePosition& start)
{
	/// TODO make a flow map of all the statics (terrain+buildings) in MapManager and use it everywhere (here included)
	set<TilePosition> endTiles;
	for (int x = max(0, pos.x()-1); x < min(Broodwar->mapWidth()/TILE_SIZE, pos.x()+ut.tileWidth()+1); ++x)
		for (int y = max(0, pos.y()-1); y < min(Broodwar->mapHeight()/TILE_SIZE, pos.y()+ut.tileHeight()+1); ++y)
			if (MapManager::Instance().isBTWalkable(x, y) && !TheReservedMap->isReserved(x, y))
				endTiles.insert(TilePosition(x, y));
	BWTA::Region*r = BWTA::getRegion(pos);
	int i = 0;
	int maxIter = static_cast<int>(r->getPolygon().getPerimeter() / TILE_SIZE); // or max dimension or region * sinuosity
	set<TilePosition> walkable;
	walkable.insert(start);
	list<TilePosition> walkableAround = manhattanWalkableNotReserved(start);
	while (!walkableAround.empty() && i++ < maxIter)
	{
		list<TilePosition> newWalkableAround;
		for each (TilePosition tp in walkableAround)
		{
			if (!walkable.count(tp))
			{
				walkable.insert(tp);
				if (BWTA::getRegion(tp) == r)
					newWalkableAround.splice(newWalkableAround.end(), manhattanWalkableNotReserved(tp));
			}
		}
		walkableAround.swap(newWalkableAround);
		bool allWalkables = true;
		for each (TilePosition tp in endTiles)
			allWalkables &= static_cast<bool>(walkable.count(tp));
		if (!allWalkables)
			continue;
		return true;
	}
	return false;
}

void PositionAccountant::cleanUp()
{
	for (list<TilePosition>::const_iterator it = pos.begin();
		it != pos.end(); )
	{
		if (SimCityBuildingPlacer::blockedBySomething(*it, ut))
			pos.erase(it++);
		else
			++it;
	}
}

TilePosition PositionAccountant::reservePos()
{
	for (list<BWAPI::TilePosition>::const_iterator it = pos.begin();
		it != pos.end(); ++it)
	{
		if (!givenPos.count(*it))
		{
			givenPos.insert(*it);
			return *it;
		}
	}
	return TilePositions::None;
}

TilePosition PositionAccountant::reservePos(TilePosition tp)
{
	if (!givenPos.count(tp))
	{
		givenPos.insert(tp);
		return tp;
	}
	return TilePositions::None;
}

TilePosition PositionAccountant::findClosest(TilePosition seed)
{
	TilePosition ret = TilePositions::None;
	double dist = DBL_MAX;
	for each (TilePosition t in pos)
	{
		if (!givenPos.count(t)
			&& t.getDistance(seed) <= dist)
		{
			dist = t.getDistance(seed);
			ret = t;
		}
	}
	if (ret != TilePositions::None)
		ret = reservePos(ret);
	return ret;
}

bool SimCityBuildingPlacer::inMineralLine(BWTA::BaseLocation* b, BWAPI::TilePosition tp)
{
	int minX = b->getTilePosition().x();
	int minY = b->getTilePosition().y();
	int maxX = b->getTilePosition().x();
	int maxY = b->getTilePosition().y();
	for each (Unit* u in b->getGeysers())
	{
		if (u->getTilePosition().x() < minX)
			minX = u->getTilePosition().x();
		if (u->getTilePosition().y() < minY)
			minY = u->getTilePosition().y();
		if (u->getTilePosition().x() > maxX)
			maxX = u->getTilePosition().x();
		if (u->getTilePosition().y() > maxY)
			maxY = u->getTilePosition().y();
	}
	for each (Unit* u in b->getMinerals())
	{
		if (u->getTilePosition().x() < minX)
			minX = u->getTilePosition().x();
		if (u->getTilePosition().y() < minY)
			minY = u->getTilePosition().y();
		if (u->getTilePosition().x() > maxX)
			maxX = u->getTilePosition().x();
		if (u->getTilePosition().y() > maxY)
			maxY = u->getTilePosition().y();
	}
	return (tp.x() < maxX && tp.x() > minX && tp.y() < maxY && tp.y() > minY);
}

PositionAccountant* SimCityBuildingPlacer::getPositionAccountant(const BWAPI::UnitType& ut)
{
    if (ut == UnitTypes::Protoss_Pylon)
		return &pylons;
    else if (ut == UnitTypes::Protoss_Gateway || ut == UnitTypes::Protoss_Stargate)
		return &gates;
	else if (ut == UnitTypes::Protoss_Photon_Cannon)
		return &cannons;
	else if (ut == UnitTypes::Protoss_Assimilator)
		return &tech; // TODO
	else
		return &tech;
}

void SimCityBuildingPlacer::generateGatesPos()
{
	for each (Base* b in TheBasesManager->getAllBases())
	{
		TilePosition tmp = generateBuildLocationNear(b->getBaseLocation()->getTilePosition(), UnitTypes::Protoss_Gateway);
		if (tmp != TilePositions::None)
			gates.addPos(tmp);
	}
}

void SimCityBuildingPlacer::generateTechPos()
{
	for each (Base* b in TheBasesManager->getAllBases())
	{
		TilePosition tmp = generateBuildLocationNear(b->getBaseLocation()->getTilePosition(), UnitTypes::Protoss_Cybernetics_Core);
		if (tmp != TilePositions::None)
			tech.addPos(tmp);
	}
}

void SimCityBuildingPlacer::generateCannonsPos()
{
	for each (BWTA::Chokepoint* cp in TheBorderManager->getMyBorder())
	{
		if (TheBorderManager->getMyRegions().count(cp->getRegions().first))
			makeCannonChoke(cp->getRegions().first, cp, true);
		else
			makeCannonChoke(cp->getRegions().second, cp, true);
	}
	// TODO
}

void SimCityBuildingPlacer::generatePylonsPos()
{
	if (TheBasesManager == NULL)
		TheBasesManager = BasesManager::create();

	clock_t start = clock(); // TODO change

	//for each (Base* bb in TheBasesManager->getAllBases())
	for each (BWTA::BaseLocation* b in BWTA::getStartLocations())
	{
		if (TheInformationManager->getEnemyBases().count(b))
			return;
		if (pylons.pos.size() > 1) // at least 2 pylons
			return;
		//BWTA::BaseLocation* b = bb->getBaseLocation();
		int minX = b->getTilePosition().x();
		int minY = b->getTilePosition().y();
		int maxX = b->getTilePosition().x();
		int maxY = b->getTilePosition().y();
		for each (Unit* u in b->getGeysers())
		{
			if (u->getTilePosition().x() < minX)
				minX = u->getTilePosition().x();
			if (u->getTilePosition().y() < minY)
				minY = u->getTilePosition().y();
			if (u->getTilePosition().x() > maxX)
				maxX = u->getTilePosition().x();
			if (u->getTilePosition().y() > maxY)
				maxY = u->getTilePosition().y();
		}
		for each (Unit* u in b->getMinerals())
		{
			if (u->getTilePosition().x() < minX)
				minX = u->getTilePosition().x();
			if (u->getTilePosition().y() < minY)
				minY = u->getTilePosition().y();
			if (u->getTilePosition().x() > maxX)
				maxX = u->getTilePosition().x();
			if (u->getTilePosition().y() > maxY)
				maxY = u->getTilePosition().y();
		}
		for (int x = max(2, b->getTilePosition().x() - 30);
			x < min(Broodwar->mapWidth() - 2, b->getTilePosition().x() + 30); ++x)
		{
			for (int y = max(2, b->getTilePosition().y() - 30);
				y < min(Broodwar->mapHeight() - 3, b->getTilePosition().y() + 30); ++y)
			{
				if (BWTA::getRegion(x, y) == b->getRegion() // same region as the base
					&& (x > maxX || x < minX || y > maxY || y < minY) // not in the mineral line
					&& canBuildHere(NULL, TilePosition(x-1, y-1), UnitTypes::Protoss_Pylon) // can walk around
					&& canBuildHere(NULL, TilePosition(x-1, y+1), UnitTypes::Protoss_Pylon)
					&& canBuildHere(NULL, TilePosition(x+1, y-1), UnitTypes::Protoss_Pylon)
					&& canBuildHere(NULL, TilePosition(x+1, y+1), UnitTypes::Protoss_Pylon))
					pylons.addPos(TilePosition(x,y));
			}
			if ((double)(clock() - start)/CLOCKS_PER_SEC > 0.01) // 1 milli second
				return; // TODO: will always stop at the same point
		}
	}
}

void SimCityBuildingPlacer::searchedAtBase(BWTA::BaseLocation* b)
{
	_searchedForClustersAtBase.insert(b);
	_searchedForClustersAtRegion.insert(b->getRegion());
}

void SimCityBuildingPlacer::generate(int min_size)
{
	if (TheBasesManager == NULL)
		TheBasesManager = BasesManager::create();

	clock_t start = clock(); // TODO change

	if (_canNoLongerGenerateClusters)
	{
		if (pylons.pos.size() < 2)
			generatePylonsPos();
		if (gates.pos.size() < 2)
			generateGatesPos();
		if (tech.pos.size() < 2)
			generateTechPos();
		if (cannons.pos.size() < 2)
			generateCannonsPos();
		return;
	}

	BuildingsCluster bc = BuildingsCluster();
	/// Search at home
	if (!_noMoreClustersAtHome && nbClusters < 5)
	{
		bc = searchForCluster(home);
		if (bc.size < 1)
			_noMoreClustersAtHome = true;
	}
	/// Search in our bases
	if (bc.size < min_size)
	{
		for (list<Base*>::const_iterator it = TheBasesManager->getAllBases().begin();
			it != TheBasesManager->getAllBases().end(); ++it)
		{
			if (!_searchedForClustersAtBase.count((*it)->getBaseLocation())
				&& (*it)->getBaseLocation()->getRegion() != home->getRegion())
				bc = searchForCluster((*it)->getBaseLocation());
			if (bc.size < 1)
				searchedAtBase((*it)->getBaseLocation());
			if (bc.size >= min_size)
				break;

			if ((double)(clock() - start)/CLOCKS_PER_SEC > 0.01) // 1 milli second
				goto generate_end; // TODO: remove
		}
	}
	/// Search at home again
	if (!_noMoreClustersAtHome && bc.size < min_size)
		bc = searchForCluster(home->getRegion()); // perhaps a 4th/5th/... cluster at home?

	if (Broodwar->getFrameCount() > 100)
		_canNoLongerGenerateClusters = true;
	/// Search at other start locations
	/*if (bc.size < min_size && Broodwar->getFrameCount() > 10*24*60)
	{
		if (TheInformationManager == NULL)
			TheInformationManager = InformationManager::create();
		for each (BWTA::BaseLocation* b in BWTA::getStartLocations())
		{
			if (b != home && !_searchedForClustersAtBase.count(b)
				&& !TheInformationManager->getEnemyBases().count(b))
			{
				bc = searchForCluster(b);
				if (bc.size < 1)
					searchedAtBase(b);
			}

			if ((double)(clock() - start)/CLOCKS_PER_SEC > 0.01) // 1 milli second
				goto generate_end; // TODO: remove
		}
	}*/

generate_end:
	if (bc.size >= min_size)
	{
		if (tech.pos.size() >= 2 && gates.pos.size() < 4)
			makeCluster(bc.center, 0, bc.vertical, bc.size);
		else
			makeCluster(bc.center, 2, bc.vertical, bc.size);
	}
#ifdef __MACRO_DEBUG__
	else
		Broodwar->printf("Could not generate a cluster of size %d", min_size);
#endif
}

set<Unit*> SimCityBuildingPlacer::checkPower(const set<Unit*>& buildings)
{
	set<Unit*> ret;
	for (set<Unit*>::const_iterator it = buildings.begin();
		it != buildings.end(); ++it)
	{
		if ((*it)->isUnpowered())
			ret.insert(*it);
	}
    return ret;
}

/*** /!\
 * Dumb heuristic, do not use (currently) elsewhere 
 * than for "buildings destruction recovery"!!
 */
bool SimCityBuildingPlacer::powerBuildings(const set<Unit*>& buildings)
{
	if (buildings.empty() || !buildings.size()) // defensive
		return true;
	/// TODO dumb heuristic: to change, may not always succeed
	Vec mid(0,0);
	for (set<Unit*>::const_iterator it = buildings.begin();
		it != buildings.end(); ++it)
		mid += (*it)->getTilePosition();
	mid /= buildings.size();
	set<Unit*> tmp = buildings;
	TilePosition topLeft(max(0, (int)mid.x - 1), max(0, (int)mid.y - 1));
	TilePosition topRight(min(Broodwar->mapWidth() - 1, (int)mid.x + 1), max(0, (int)mid.y - 1)); // mapWidth() - 1 because the building TilePosition is the top left
	TilePosition botLeft(max(0, (int)mid.x - 1), min(Broodwar->mapHeight() - 2, (int)mid.y + 1)); // mapHeight() - 2 because the bottom line Tiles are not buildable
	TilePosition botRight(min(Broodwar->mapWidth() - 1, (int)mid.x + 1), min(Broodwar->mapHeight() - 2, (int)mid.y + 1));
	TilePosition midTp = mid.toTilePosition();
	if (midTp.hasPath(TilePosition(home->getTilePosition().x() - 1, home->getTilePosition().y() - 1))
		&& Broodwar->canBuildHere(NULL, midTp, UnitTypes::Protoss_Pylon)
		&& Broodwar->canBuildHere(NULL, topLeft, UnitTypes::Protoss_Pylon) // check if we can walk all around
		&& Broodwar->canBuildHere(NULL, topRight, UnitTypes::Protoss_Pylon)
		&& Broodwar->canBuildHere(NULL, botLeft, UnitTypes::Protoss_Pylon)
		&& Broodwar->canBuildHere(NULL, botRight, UnitTypes::Protoss_Pylon))
	{
		TheBuilder->build(UnitTypes::Protoss_Pylon, midTp);
		set<Unit*> inRadius = Broodwar->getUnitsInRadius(mid.toPosition(), 32*6);
		for (set<Unit*>::const_iterator it = inRadius.begin();
			it != inRadius.end(); ++it)
			tmp.erase(*it);
	}
	int count = 0;
	int maxCount = tmp.size();
	while (tmp.size() && count < maxCount)
	{
		// try left
		int xTp = min(0, (*tmp.begin())->getTilePosition().x() - 1);
		mid = Vec(xTp, (*tmp.begin())->getTilePosition().y());
		TilePosition topLeft(max(0, (int)mid.x - 1), max(0, (int)mid.y - 1));
		TilePosition botLeft(max(0, (int)mid.x - 1), min(Broodwar->mapHeight() - 2, (int)mid.y + 1)); // mapHeight() - 2 because the bottom line Tiles are not buildable
		midTp = mid.toTilePosition();
		if (midTp.hasPath(TilePosition(home->getTilePosition().x() - 1, home->getTilePosition().y() - 1))
			&& Broodwar->canBuildHere(NULL, midTp, UnitTypes::Protoss_Pylon)
			&& Broodwar->canBuildHere(NULL, topLeft, UnitTypes::Protoss_Pylon) // check if we can walk around
			&& Broodwar->canBuildHere(NULL, botLeft, UnitTypes::Protoss_Pylon))
		{
			TheBuilder->build(UnitTypes::Protoss_Pylon, midTp);
			set<Unit*> inRadius = Broodwar->getUnitsInRadius(mid.toPosition(), 32*6);
			for (set<Unit*>::const_iterator it = inRadius.begin();
				it != inRadius.end(); ++it)
				tmp.erase(*it);
		}
		else
		{
			// try right
			xTp = max(Broodwar->mapWidth(), (*tmp.begin())->getTilePosition().x() + (*tmp.begin())->getType().tileWidth());
			mid = Vec(xTp, (*tmp.begin())->getTilePosition().y());
			TilePosition topRight(min(Broodwar->mapWidth() - 1, (int)mid.x + 1), max(0, (int)mid.y - 1)); // mapWidth() - 1 because the building TilePosition is the top left
			TilePosition botRight(min(Broodwar->mapWidth() - 1, (int)mid.x + 1), min(Broodwar->mapHeight() - 2, (int)mid.y + 1));
			midTp = mid.toTilePosition();
			if (midTp.hasPath(TilePosition(home->getTilePosition().x() - 1, home->getTilePosition().y() - 1))
				&& Broodwar->canBuildHere(NULL, mid.toTilePosition(), UnitTypes::Protoss_Pylon)
				&& Broodwar->canBuildHere(NULL, topRight, UnitTypes::Protoss_Pylon) // check if we can walk around
				&& Broodwar->canBuildHere(NULL, botRight, UnitTypes::Protoss_Pylon))
			{
				TheBuilder->build(UnitTypes::Protoss_Pylon, midTp);
				set<Unit*> inRadius = Broodwar->getUnitsInRadius(mid.toPosition(), 32*6);
				for (set<Unit*>::const_iterator it = inRadius.begin();
					it != inRadius.end(); ++it)
					tmp.erase(*it);
			}
		}
		++count;
	}
	return tmp.empty();
}

BuildingsCluster SimCityBuildingPlacer::searchForCluster(BWTA::BaseLocation* b)
{
	return searchForCluster(b->getRegion());
}

BuildingsCluster SimCityBuildingPlacer::searchForCluster(BWTA::Region* r)
{
	int minX = INT_MAX;
	int minY = INT_MAX;
	int maxX = 0;
	int maxY = 0;
	for (vector<Position>::const_iterator it = r->getPolygon().begin();
		it != r->getPolygon().end(); ++it)
	{   
		int tmpX = it->x()/32;
		int tmpY = it->y()/32;
		if (tmpX < minX)
			minX = tmpX;
		if (tmpX > maxX)
		    maxX = tmpX;
		if (tmpY < minY)
			minY = tmpY;
		if (tmpY > maxY)
		    maxY = tmpY;
	}
	return searchForCluster(minX, maxX, minY, maxY, r);
}

pair<TilePosition, TilePosition> getOuterBoundsRegion(BWTA::Region* r)
{
	TilePosition center(r->getCenter());
	int minX = center.x();
	int maxX = center.x();
	int minY = center.y();
	int maxY = center.y();
	for each (Position p in r->getPolygon())
	{
		if (p.x()/TILE_SIZE < minX)
			minX = p.x()/TILE_SIZE;
		if (p.y()/TILE_SIZE < minY)
			minY = p.y()/TILE_SIZE;
		if (p.x()/TILE_SIZE > maxX)
			maxX = p.x()/TILE_SIZE;
		if (p.y()/TILE_SIZE > maxY)
			maxY = p.y()/TILE_SIZE;
	}
	return make_pair(TilePosition(minX, minY), TilePosition(maxX, maxY));
}

BuildingsCluster SimCityBuildingPlacer::searchForCluster(int minX, int maxX, int minY, int maxY, BWTA::Region* r)
{
	int minXs = max(1, minX);
	int minYs = max(1, minY);
	int maxXs = min(Broodwar->mapWidth() - 1, maxX);
	int maxYs = min(Broodwar->mapHeight() - 2, maxY); // the bottom line is not buildable
	BuildingsCluster ret;
	ret.size = 0;
	/// search for "big clusters"
	int minXClusterDim = UnitTypes::Protoss_Pylon.tileWidth() + 2*UnitTypes::Protoss_Gateway.tileWidth() + 2; // 2 to move around
	int minYClusterDim = UnitTypes::Protoss_Pylon.tileHeight() + 2*UnitTypes::Protoss_Gateway.tileHeight() + 2;
	int tmpMaxX = maxXs - minXClusterDim;
	int tmpMaxY = maxYs - minYClusterDim;
	for (int x = minXs; x < tmpMaxX; x += 1)
		for (int y = minYs; y < tmpMaxY; y += 1)
		{
			TilePosition topLeft(x, y);
			if (BWTA::getRegion(topLeft) != r)
				continue;
			TilePosition topRight(x + minXClusterDim, y);
			if (BWTA::getRegion(topRight) != r)
				continue;
			TilePosition botLeft(x, y + minYClusterDim);
			if (BWTA::getRegion(botLeft) != r)
				continue;
			TilePosition botRight(x + minXClusterDim, y + minYClusterDim);
			if (BWTA::getRegion(botRight) != r)
				continue;
			if (canBuildHere(NULL, topLeft, UnitTypes::Protoss_Pylon)
				&& canBuildHere(NULL, topRight, UnitTypes::Protoss_Pylon)
				&& canBuildHere(NULL, botLeft, UnitTypes::Protoss_Pylon)
				&& canBuildHere(NULL, botRight, UnitTypes::Protoss_Pylon))
			{
				ret.center = TilePosition(topLeft.x() + UnitTypes::Protoss_Gateway.tileWidth() + 1, topLeft.y() + 2*UnitTypes::Protoss_Pylon.tileHeight() + 1);
				ret.vertical = true;
				ret.size = canBuildCluster(ret.center, ret.vertical);
				if (ret.size > 2)
					return ret;
				ret.center = TilePosition(topLeft.x() + 3*UnitTypes::Protoss_Pylon.tileWidth() + 1, topLeft.y() + UnitTypes::Protoss_Gateway.tileHeight() + 1);
				ret.vertical = false;
				ret.size = canBuildCluster(ret.center, ret.vertical);
				if (ret.size > 2)
					return ret;
			}
		}
	/// search for "small clusters"
	minXClusterDim = 2*UnitTypes::Protoss_Gateway.tileWidth() + 2; // 2 to move around
	minYClusterDim = 2*UnitTypes::Protoss_Gateway.tileHeight() + 2;
	tmpMaxX = maxXs - minXClusterDim;
	tmpMaxY = maxYs - minYClusterDim;
	for (int x = minXs; x < tmpMaxX; x += 1)
		for (int y = minYs; y < tmpMaxY; y += 1)
		{
			TilePosition topLeft(x, y);
			if (BWTA::getRegion(topLeft) != r)
				continue;
			TilePosition topRight(x + minXClusterDim, y);
			if (BWTA::getRegion(topRight) != r)
				continue;
			TilePosition botLeft(x, y + minYClusterDim);
			if (BWTA::getRegion(botLeft) != r)
				continue;
			TilePosition botRight(x + minXClusterDim, y + minYClusterDim);
			if (BWTA::getRegion(botRight) != r)
				continue;
			if (canBuildHere(NULL, topLeft, UnitTypes::Protoss_Pylon)
				&& canBuildHere(NULL, topRight, UnitTypes::Protoss_Pylon)
				&& canBuildHere(NULL, botLeft, UnitTypes::Protoss_Pylon)
				&& canBuildHere(NULL, botRight, UnitTypes::Protoss_Pylon))
			{
				ret.center = TilePosition(topLeft.x() + UnitTypes::Protoss_Gateway.tileWidth() + 1, topLeft.y() + UnitTypes::Protoss_Pylon.tileHeight() + 1);
				ret.vertical = true;
				ret.size = canBuildCluster(ret.center, ret.vertical);
				if (ret.size > 1) // return as soon as a small cluster is found
					return ret;
				ret.center = TilePosition(topLeft.x() + 2*UnitTypes::Protoss_Pylon.tileWidth() + 1, topLeft.y() + UnitTypes::Protoss_Gateway.tileHeight() + 1);
				ret.vertical = false;
				ret.size = canBuildCluster(ret.center, ret.vertical);
				if (ret.size > 1) // return as soon as a small cluster is found
					return ret;
			}
		}
	/// search for 2 buildings clusters (smallest)
	minXClusterDim = 2*UnitTypes::Protoss_Pylon.tileWidth() + 2; // 2 to move around
	minYClusterDim = 2*UnitTypes::Protoss_Pylon.tileHeight() + 2;
	tmpMaxX = maxXs - minXClusterDim;
	tmpMaxY = maxYs - minYClusterDim;
	for (int x = minXs; x < tmpMaxX; x += 1)
		for (int y = minYs; y < tmpMaxY; y += 1)
		{
			TilePosition topLeft(x, y);
			if (BWTA::getRegion(topLeft) != r)
				continue;
			TilePosition topRight(x + minXClusterDim, y);
			if (BWTA::getRegion(topRight) != r)
				continue;
			TilePosition botLeft(x, y + minYClusterDim);
			if (BWTA::getRegion(botLeft) != r)
				continue;
			TilePosition botRight(x + minXClusterDim, y + minYClusterDim);
			if (BWTA::getRegion(botRight) != r)
				continue;
			if (canBuildHere(NULL, topLeft, UnitTypes::Protoss_Pylon)
				&& canBuildHere(NULL, topRight, UnitTypes::Protoss_Pylon)
				&& canBuildHere(NULL, botLeft, UnitTypes::Protoss_Pylon)
				&& canBuildHere(NULL, botRight, UnitTypes::Protoss_Pylon))
			{
				ret.center = TilePosition(topLeft.x() + UnitTypes::Protoss_Gateway.tileWidth() + 1, topLeft.y() + UnitTypes::Protoss_Pylon.tileHeight() + 1);
				ret.vertical = true;
				ret.size = canBuildCluster(ret.center, ret.vertical);
				if (ret.size) // return as soon as a small cluster is found
					return ret;
				ret.center = TilePosition(topLeft.x() + UnitTypes::Protoss_Pylon.tileWidth() + 1, topLeft.y() + UnitTypes::Protoss_Gateway.tileHeight() + 1);
				ret.vertical = false;
				ret.size = canBuildCluster(ret.center, ret.vertical);
				if (ret.size) // return as soon as a small cluster is found
					return ret;
			}
		}
	return ret;
}

bool SimCityBuildingPlacer::canBuildCluster(TilePosition center, bool vertical, int size)
{
	if (vertical)
	{
		int minX = center.x() - UnitTypes::Protoss_Gateway.tileWidth() - 1; // -1 additional tile to move around
		if (minX <= 0)
			return 0;
		int maxX = center.x() + UnitTypes::Protoss_Pylon.tileWidth() + UnitTypes::Protoss_Gateway.tileWidth();
		if (maxX >= Broodwar->mapWidth() - 1)
			return 0;
		int i = 1; // size == 1
		int j = 0; // size == 1
		if (size > 1)
		{
			i = size;
			j = size;
		}
		int minY = center.y() - i*UnitTypes::Protoss_Pylon.tileHeight() - 1;
		int maxY = center.y() + j*UnitTypes::Protoss_Pylon.tileHeight() + 1;
		if (minY <= 0 || maxY >= Broodwar->mapHeight() - 2)
			return false;
		for (int x = minX; x <= maxX; ++x)
		{
			for (int y = minY; y <= maxY; ++y)
			{
				if (!canBuildHere(NULL, TilePosition(x, y), UnitTypes::Protoss_Pylon))
					return false;
			}
		}
		return true;
	}
	else
	{
		int minY = center.y() - UnitTypes::Protoss_Gateway.tileHeight() - 1; // -1 additional tile to move around
		if (minY <= 0)
			return false;
		int maxY = center.y() + UnitTypes::Protoss_Pylon.tileHeight() + UnitTypes::Protoss_Gateway.tileHeight();
		if (maxY >= Broodwar->mapHeight() - 2)
			return false;
		int minX = center.x() - size*UnitTypes::Protoss_Pylon.tileWidth() - 1;
		int maxX = center.x() + (size-1)*UnitTypes::Protoss_Pylon.tileWidth() + 1;
		if (minX <= 0 || maxX >= Broodwar->mapWidth() - 1)
			return false;
		for (int x = minX; x <= maxX; ++x)
		{
			for (int y = minY; y <= maxY; ++y)
			{
				if (!canBuildHere(NULL, TilePosition(x, y), UnitTypes::Protoss_Pylon))
			         return false;
			}
		}
	    return true;
	}
}

/***
 * Returns either false (0) or the size (1=small, 2=big) of the cluster
 * one can build at around "center"
 */
int SimCityBuildingPlacer::canBuildCluster(const TilePosition& center, bool vertical)
{
	if (canBuildCluster(center, vertical, 3))
		return 3;
	else if (canBuildCluster(center, vertical, 2))
		return 2;
	else if (canBuildCluster(center, vertical, 1))
		return 1;
	else
		return 0;
}

int SimCityBuildingPlacer::makeCluster(const TilePosition& center, int nbTechBuildings, bool vertical, int cSize)
{
	int clusterSize = 0;
	if (!cSize)
		clusterSize = canBuildCluster(center, vertical);
	else
		clusterSize = cSize;
	if (!clusterSize)
		return 0;
	int techToPlace = min(4, nbTechBuildings);
	int nbGates = clusterSize*2 - techToPlace;
	int height = 0;
	int width = 0;
	if (vertical)
	{
		height = clusterSize * UnitTypes::Protoss_Gateway.tileHeight();
		if (techToPlace > 1)
			--height;
		if (techToPlace > 3)
			--height;
		if (height % 2)
			++height;
		width = 2*UnitTypes::Protoss_Gateway.tileWidth() + UnitTypes::Protoss_Pylon.tileWidth();
		/// place pylons
		int topLeft = center.y();
		int nbPylons = height / 2; 
		pylons.addAsSecondPos(center);
		--nbPylons;
		int maxi = (nbPylons+1)/2;
		for (int i = 1; i <= maxi; ++i)
		{
			if (nbPylons > 0)
			{
				topLeft = center.y() - i*UnitTypes::Protoss_Pylon.tileHeight();
				pylons.addPos(TilePosition(center.x(), topLeft));
				--nbPylons;
			}
			else
				break;
			if (nbPylons > 0)
			{
				pylons.addPos(TilePosition(center.x(), center.y() + i*UnitTypes::Protoss_Pylon.tileHeight()));
				--nbPylons;
			}
			else
				break;
		}
		/// place tech buildings (left to right)
		multimap<unsigned int, TilePosition> techBuildings;
		maxi = (techToPlace+1)/2;
		int topRight = topLeft;
		for (int i = 0; i <= maxi; ++i)
		{
			if (techToPlace)
			{
				techBuildings.insert(make_pair<unsigned int, TilePosition>(abs(topLeft-center.y()), TilePosition(center.x() - UnitTypes::Protoss_Cybernetics_Core.tileWidth(), topLeft)));
				topLeft += UnitTypes::Protoss_Cybernetics_Core.tileHeight();
				--techToPlace;
			}
			else
				break;
			if (techToPlace)
			{
				techBuildings.insert(make_pair<unsigned int, TilePosition>(abs(topRight-center.y()), TilePosition(center.x() + UnitTypes::Protoss_Pylon.tileWidth(), topRight)));
				topRight += UnitTypes::Protoss_Cybernetics_Core.tileHeight();
				--techToPlace;
			}
			else
				break;
		}
		for (multimap<unsigned int, TilePosition>::const_iterator it = techBuildings.begin();
			it != techBuildings.end(); ++it)
			tech.addPos(it->second);
		/// place gates (RIGHT to left, to complement and odd number of tech buildings)
		multimap<unsigned int, TilePosition> gatesBuildings;
		maxi = (nbGates+1)/2;
		for (int i = 0; i <= maxi; ++i)
		{
			if (nbGates)
			{
				gatesBuildings.insert(make_pair<unsigned int, TilePosition>(abs(topRight-center.y()), TilePosition(center.x() + UnitTypes::Protoss_Pylon.tileWidth(), topRight)));
				topRight += UnitTypes::Protoss_Gateway.tileHeight();
				--nbGates;
			}
			else
				break;
			if (nbGates)
			{
				gatesBuildings.insert(make_pair<unsigned int, TilePosition>(abs(topLeft-center.y()), TilePosition(center.x() - UnitTypes::Protoss_Gateway.tileWidth(), topLeft)));
				topLeft += UnitTypes::Protoss_Gateway.tileHeight();
				--nbGates;
			}
			else
				break;
		}
		for (multimap<unsigned int, TilePosition>::const_iterator it = gatesBuildings.begin();
			it != gatesBuildings.end(); ++it)
			gates.addPos(it->second);
	}
	else
	{
		width = clusterSize * UnitTypes::Protoss_Gateway.tileWidth();
		if (techToPlace > 1)
			--width;
		if (techToPlace > 3)
			--width;
		if (width % 2)
			++width;
		height = 2*UnitTypes::Protoss_Gateway.tileHeight() + UnitTypes::Protoss_Pylon.tileHeight();
		/// place pylons
		int leftTop = center.x();
		int nbPylons = width / 2;
		pylons.addAsSecondPos(center);
		--nbPylons;
		int maxi = (nbPylons+1)/2;
		for (int i = 1; i <= maxi; ++i)
		{
			if (nbPylons)
			{
				leftTop = center.x() - i*UnitTypes::Protoss_Pylon.tileWidth();
				pylons.addPos(TilePosition(leftTop, center.y()));
				--nbPylons;
			}
			else
				break;
			if (nbPylons)
			{
				pylons.addPos(TilePosition(center.x() + i*UnitTypes::Protoss_Pylon.tileWidth(), center.y()));
				--nbPylons;
			}
			else
				break;
		}
		/// place tech buildings (top to bottom)
		multimap<unsigned int, TilePosition> techBuildings;
		maxi = (techToPlace+1)/2;
		int leftBot = leftTop;
		for (int i = 0; i <= maxi; ++i)
		{
			if (techToPlace)
			{
				techBuildings.insert(make_pair<unsigned int, TilePosition>(abs(leftTop-center.x()), TilePosition(leftTop,
					center.y() - UnitTypes::Protoss_Cybernetics_Core.tileHeight())));
				leftTop += UnitTypes::Protoss_Cybernetics_Core.tileWidth();
				--techToPlace;
			}
			else
				break;
			if (techToPlace)
			{
				techBuildings.insert(make_pair<unsigned int, TilePosition>(abs(leftBot-center.x()), TilePosition(leftBot,
					center.y() + UnitTypes::Protoss_Pylon.tileHeight())));
				leftBot += UnitTypes::Protoss_Cybernetics_Core.tileWidth();
				--techToPlace;
			}
			else
				break;
		}
		for (multimap<unsigned int, TilePosition>::const_iterator it = techBuildings.begin();
			it != techBuildings.end(); ++it)
			tech.addPos(it->second);
		/// place gates (BOTTOM to top, to complement and odd number of tech buildings)
		multimap<unsigned int, TilePosition> gatesBuildings;
		maxi = (nbGates+1)/2;
		for (int i = 0; i <= maxi; ++i)
		{
			if (nbGates)
			{
				gatesBuildings.insert(make_pair<unsigned int, TilePosition>(abs(leftBot-center.x()), TilePosition(leftBot,
					center.y() + UnitTypes::Protoss_Pylon.tileHeight())));
				leftBot += UnitTypes::Protoss_Gateway.tileWidth();
				--nbGates;
			}
			else
				break;
			if (nbGates)
			{
				gatesBuildings.insert(make_pair<unsigned int, TilePosition>(abs(leftTop-center.x()), TilePosition(leftTop,
					center.y() - UnitTypes::Protoss_Gateway.tileHeight())));
				leftTop += UnitTypes::Protoss_Gateway.tileWidth();
				--nbGates;
			}
			else
				break;
		}
		for (multimap<unsigned int, TilePosition>::const_iterator it = gatesBuildings.begin();
			it != gatesBuildings.end(); ++it)
			gates.addPos(it->second);
	}
	++nbClusters;
	return clusterSize;
}


/***
 * As inefficient as possible
 */
TilePosition SimCityBuildingPlacer::closestBuildableSameRegion(const TilePosition& tp)
{
	BWTA::Region* r = BWTA::getRegion(tp);
	for (int searchDim = 1; searchDim < 10; ++searchDim)
		for (int x = -searchDim; x <= searchDim; ++x)
			for (int y = -searchDim; y <= searchDim; ++y)
			{
				TilePosition tmp(tp.x() + x, tp.y() + y);
				if (BWTA::getRegion(tmp) != r)
					continue;
				if (canBuildHere(NULL, tmp, UnitTypes::Protoss_Pylon))
					return tmp;
			}
	return TilePositions::None;
}

/***
 * As copy/pasted as possible
 */
TilePosition SimCityBuildingPlacer::closestBuildableSameRegionNotTP2(const TilePosition& tp, const TilePosition& tp2)
{
	BWTA::Region* r = BWTA::getRegion(tp);
	for (int searchDim = 1; searchDim < 10; ++searchDim)
		for (int x = -searchDim; x <= searchDim; ++x)
			for (int y = -searchDim; y <= searchDim; ++y)
			{
				TilePosition tmp(tp.x() + x, tp.y() + y);
				tmp.makeValid();
				if (BWTA::getRegion(tmp) != r)
					continue;
				if (tmp == tp2)
					continue;
				if (abs(x-tp2.x() < 2) || abs(y-tp2.y()) < 2)
					continue;
				if (canBuildHere(NULL, tmp, UnitTypes::Protoss_Pylon))
					return tmp;
			}
	return TilePositions::None;
}

/*** /!\ SHITTY HEURISTIC
 * If "quick", it puts the pylon quicker (second position of pylons)
 */
void SimCityBuildingPlacer::makeCannonChoke(BWTA::Region* inter, BWTA::Chokepoint* chok, bool quick)
{
	BWTA::Region* exter = chok->getRegions().first == inter ? chok->getRegions().second : chok->getRegions().first;
	Position side1 = chok->getSides().first;
	Position side2 = chok->getSides().second;
	Vec dir(side1.x() - side2.x(), side1.y() - side2.y());
	Vec parallel(dir); // save direction parallel to the choke
	// rotate 90 deg CW
	double tmp = dir.x;
	dir.x = dir.y;
	dir.y = -tmp;
	dir = dir.normalize();
	dir *= 2 * TILE_SIZE;
	Position interP = Positions::None;
	Position exterP = Positions::None;
	/// Find two valid Positions for interP and exterP
	while ((interP == Positions::None || exterP == Positions::None) && dir.norm() < 5 * chok->getWidth())
	{
		dir *= 2;
		Position p1 = dir.translate(chok->getCenter());
		Vec tmp(- dir.x, - dir.y);
		Position p2 = tmp.translate(chok->getCenter());
		interP = BWTA::getRegion(TilePosition(p1)) == inter ? p1 : p2;
		exterP = BWTA::getRegion(TilePosition(p1)) == exter ? p1 : p2;
	}
	if (interP == Positions::None || exterP == Positions::None)
	{
		// we failed to determine internal and external Positions
#ifdef __MACRO_DEBUG__
		Broodwar->printf("makeCannonChoke failed to determine internal and external positions of the choke");
#endif
		return;
	}
	/// Find an interTP seed TilePosition to build from
	TilePosition interTP(interP);
	TilePosition exterTP(exterP);
	if (!canBuildHere(NULL, interTP, UnitTypes::Protoss_Pylon))
		interTP = closestBuildableSameRegion(interTP);
	if (interTP == TilePositions::None)
	{
		// failed again! :P
#ifdef __MACRO_DEBUG__
		Broodwar->printf("makeCannonChoke failed to determine a buildable interTP tile");
#endif
		return;
	}
	Vec simpleDirPara = abs(parallel.x) > abs(parallel.y) ? Vec(parallel.x, 0) : Vec(0, parallel.y);
	simpleDirPara = simpleDirPara.normalize() * 2; // cannon and pylons are 2 tiles high and large
	Vec fromInterToExter(exterTP.x() - interTP.x(), exterTP.y() - interTP.y());
	fromInterToExter = fromInterToExter.normalize();
	fromInterToExter *= 1.6; // > sqrt(2) && < 2
	Vec fromExterToInter(-exterTP.x() + interTP.x(), -exterTP.y() + interTP.y());
	fromExterToInter = fromExterToInter.normalize();
	fromExterToInter *= 1.6; // > sqrt(2) && < 2
	TilePosition inner((Vec(interTP.x(), interTP.y()) + fromExterToInter + simpleDirPara).toTilePosition());
	if (!canBuildHere(NULL, inner, UnitTypes::Protoss_Pylon))
		inner = closestBuildableSameRegion(inner);

	TilePosition pylon(interTP);
	TilePosition cannon((simpleDirPara+Vec(interTP.x(), interTP.y())).toTilePosition());
	if (!canBuildHere(NULL, cannon, UnitTypes::Protoss_Pylon))
		cannon = closestBuildableSameRegionNotTP2(cannon, pylon);

	set<TilePosition> tmpSet;
	tmpSet.insert(pylon);
	tmpSet.insert(cannon);
	/// TODO see if blocked and if so try to move around the seed (interTP)
	/*while (!existsInnerPath(inner, exterTP, tmpSet) && pylon.getDistance(interTP) < 3)
	{
		tmpSet.clear();
		tmpSet.insert(pylon);
		tmpSet.insert(cannon);
	}*/

	/*if (!existsInnerPath(inner, exterTP, tmpSet))
	{
		/// Try exterior then... (simpler)
		pylon = exterTP;
		cannon = ((simpleDirPara+Vec(exterTP.x(), exterTP.y())).toTilePosition());
		tmpSet.clear();
		tmpSet.insert(pylon);
		tmpSet.insert(cannon);
		TilePosition exxer((Vec(exterTP.x(), exterTP.y()) + fromInterToExter + simpleDirPara).toTilePosition());
		if (!existsInnerPath(inner, exxer, tmpSet))
		{
			pylon = TilePositions::None;
			cannon = TilePositions::None;
		}
	}*/

	/// If not none and canBuild, then add at last...
	if (pylon != TilePositions::None && cannon != TilePositions::None
		&& canBuildHere(NULL, pylon, UnitTypes::Protoss_Pylon)
		&& canBuildHere(NULL, cannon, UnitTypes::Protoss_Pylon))
	{
		if (quick)
			pylons.addAsSecondPos(pylon);
		else
			pylons.addPos(pylon);
		cannons.addPos(cannon);
	}
}

/***
 * If "quick", it puts the pylon quicker (second position of pylons)
 */
void SimCityBuildingPlacer::makeCannonsMinerals(BWTA::BaseLocation* hom, bool quick)
{
	int x = 0;
	int y = 0;
	for (set<Unit*>::const_iterator it = hom->getMinerals().begin();
		it != hom->getMinerals().end(); ++it)
	{
		x += (*it)->getTilePosition().x();
		y += (*it)->getTilePosition().y();
	}
	TilePosition gasTilePos;
	for (set<Unit*>::const_iterator it = hom->getGeysers().begin();
		it != hom->getGeysers().end(); ++it)
	{
		gasTilePos = (*it)->getTilePosition();
		x += gasTilePos.x();
		y += gasTilePos.y();
	}
	if (x == 0 || y == 0)
		return;
	TilePosition meanMineral(x / (hom->getMinerals().size()+hom->getGeysers().size()),
		y / (hom->getMinerals().size()+hom->getGeysers().size()));
	double dist = 0.0;
	TilePosition furtherMineral;
	for (set<Unit*>::const_iterator it = hom->getMinerals().begin();
		it != hom->getMinerals().end(); ++it)
	{
		double tmp = meanMineral.getDistance((*it)->getTilePosition());
		if (tmp > dist)
		{
			dist = tmp;
			furtherMineral = (*it)->getTilePosition();
		}
	}
	dist = DBL_MAX;
	double distb = DBL_MAX;
	double dist2 = DBL_MAX;
	double dist2b = DBL_MAX;
	TilePosition cannon1;
	TilePosition cannon2;
	TilePosition pylon1;
	TilePosition pylon2;
	for (x = min(max(gasTilePos.x() - UnitTypes::Protoss_Pylon.tileWidth(), 0), max(furtherMineral.x() - UnitTypes::Protoss_Pylon.tileWidth(), 0));
		x < max(min(gasTilePos.x() + UnitTypes::Resource_Vespene_Geyser.tileWidth() + UnitTypes::Protoss_Pylon.tileWidth() + 1, Broodwar->mapWidth()),
		min(furtherMineral.x() + UnitTypes::Resource_Mineral_Field.tileWidth() + UnitTypes::Protoss_Pylon.tileWidth() + 1, Broodwar->mapWidth())); x += 2)
		for (y = min(max(gasTilePos.y() - UnitTypes::Protoss_Pylon.tileHeight(), 0), max(furtherMineral.y() - UnitTypes::Protoss_Pylon.tileHeight(), 0));
			y < max(min(gasTilePos.y() + UnitTypes::Resource_Vespene_Geyser.tileHeight() + UnitTypes::Protoss_Pylon.tileHeight() + 1, Broodwar->mapHeight()),
			min(furtherMineral.y() + UnitTypes::Resource_Mineral_Field.tileHeight() + UnitTypes::Protoss_Pylon.tileHeight() + 1, Broodwar->mapHeight())); y += 2)
		{
			TilePosition tmp(x, y);
			if (inMineralLine(hom, tmp))
				continue;
			if (tmp.getDistance(hom->getTilePosition()) < 6)
				continue;
			double tmpd1 = tmp.getDistance(furtherMineral);
			if (tmpd1 < dist && canBuildHereWithPath(NULL, tmp, UnitTypes::Protoss_Pylon))
			{
				pylon1 = cannon1;
				cannon1 = tmp;
				dist = tmpd1;
			}
			else if (tmpd1 < distb && canBuildHereWithPath(NULL, tmp, UnitTypes::Protoss_Pylon))
			{
				pylon1 = tmp;
				distb = tmpd1;
			}
			double tmpd2 = tmp.getDistance(gasTilePos);
			if (tmpd2 < dist2 && canBuildHereWithPath(NULL, tmp, UnitTypes::Protoss_Pylon))
			{
				pylon2 = cannon2;
				cannon2 = tmp;
				dist2 = tmpd2;
			}
			else if (tmpd2 < dist2b && canBuildHereWithPath(NULL, tmp, UnitTypes::Protoss_Pylon))
			{
				pylon2 = tmp;
				dist2b = tmpd2;
			}
		}
	if (quick)
	{
		pylons.addAsSecondPos(pylon2); // order voluntary
		pylons.addAsSecondPos(pylon1);
	}
	else
	{
		pylons.addPos(pylon1);
		pylons.addPos(pylon2);
	}
	cannons.addPos(cannon1);
	cannons.addPos(cannon2);
}

SimCityBuildingPlacer::SimCityBuildingPlacer()
: home(BWTA::getStartLocation(Broodwar->self()))
, nbClusters(0)
, pylons(UnitTypes::Protoss_Pylon)
, gates(UnitTypes::Protoss_Gateway)
, cannons(UnitTypes::Protoss_Photon_Cannon)
, tech(UnitTypes::Protoss_Cybernetics_Core)
, _noMoreClustersAtHome(false)
, _canNoLongerGenerateClusters(false)
{
	/// search and save front and backdoor chokes
	backdoorChokes = home->getRegion()->getChokepoints();
	double minDist = DBL_MAX;
	set<BWTA::BaseLocation*> startLocs = BWTA::getStartLocations();
	startLocs.erase(BWTA::getStartLocation(Broodwar->self()));
#ifdef __MACRO_DEBUG__
	assert(!startLocs.empty()); // 1 player map ? :D
#endif
	TilePosition otherStartLocation((*startLocs.begin())->getTilePosition());
	for (set<BWTA::Chokepoint*>::const_iterator it = backdoorChokes.begin();
		it != backdoorChokes.end(); ++it)
	{
		double tmpDist = BWTA::getGroundDistance(TilePosition((*it)->getCenter()), 
			otherStartLocation); // problem with island maps...
		if (tmpDist < minDist)
		{
			minDist = tmpDist;
			frontChoke = *it;
		}
	}
	Micro::Instance().ourChokes = backdoorChokes; // TODO change/remove
	Micro::Instance().frontChoke = frontChoke;    // TODO change/remove
	backdoorChokes.erase(frontChoke);

	TilePosition nexus = home->getTilePosition();
	TilePosition front = TilePosition(frontChoke->getCenter());

	if (TheReservedMap == NULL)
		TheReservedMap = ReservedMap::create();
	for each (BWTA::BaseLocation* b in BWTA::getBaseLocations())
	{
		TheReservedMap->reserveTiles(b->getTilePosition(), UnitTypes::Protoss_Nexus, 
		    UnitTypes::Protoss_Nexus.tileWidth(), UnitTypes::Protoss_Nexus.tileHeight());
		/*for each (Unit* u in b->getGeysers())
			TheReservedMap->reserveTiles(u->getTilePosition(), UnitTypes::Resource_Vespene_Geyser, 
			    UnitTypes::Resource_Vespene_Geyser.tileWidth(), UnitTypes::Resource_Vespene_Geyser.tileHeight());
		for each (Unit* u in b->getMinerals())
			TheReservedMap->reserveTiles(u->getTilePosition(), UnitTypes::Resource_Mineral_Field, 
			    UnitTypes::Resource_Mineral_Field.tileWidth(), UnitTypes::Resource_Mineral_Field.tileHeight());
				*/
	}
	for each (BWTA::BaseLocation* b in BWTA::getStartLocations()) // bug in the previous loop (just above)?
	{
		if (TheReservedMap->isReserved(b->getTilePosition()))
			continue;
		TheReservedMap->reserveTiles(b->getTilePosition(), UnitTypes::Protoss_Nexus, 
		    UnitTypes::Protoss_Nexus.tileWidth(), UnitTypes::Protoss_Nexus.tileHeight());
	}

	/// best place to do a pylons/gates cluster
	Vec dir(front.x() - nexus.x(), front.y() - nexus.y());
	bool vertical = abs((int)dir.y) > abs((int)dir.x);
	Vec nex(nexus.x(), nexus.y());
	dir = dir.normalize();
	dir *= 2*UnitTypes::Protoss_Gateway.tileWidth() + UnitTypes::Protoss_Pylon.tileWidth();
	nex = nex.vecTranslate(dir);
	TilePosition cluster_center(nex.toTilePosition());

	BuildingsCluster bc; bc.size = 0;
	bc = searchForCluster(cluster_center.x() - 2, cluster_center.y() - 2, cluster_center.x() + 2, cluster_center.y() + 2, home->getRegion());
	if (bc.size < 2)
		generate(2);
	else
		makeCluster(bc.center, 2, bc.vertical, bc.size);
	generate();

	/// search places to put cannons at chokes
	makeCannonChoke(home->getRegion(), frontChoke, true);

	/// search places to put cannons behind minerals
	makeCannonsMinerals(home);
}

#ifdef __MACRO_DEBUG__
void SimCityBuildingPlacer::update()
{
	int i = 0;
	for (list<TilePosition>::const_iterator it = pylons.pos.begin();
		it != pylons.pos.end(); ++it)
	{
		Broodwar->drawBoxMap(it->x()*32, it->y()*32, (it->x()+2)*32, (it->y()+2)*32, Colors::Yellow);
		Broodwar->drawTextMap(it->x()*32+2, it->y()*32+2, "%d", i++);
	}
	i = 0;
	for (list<TilePosition>::const_iterator it = gates.pos.begin();
		it != gates.pos.end(); ++it)
	{
		Broodwar->drawBoxMap(it->x()*32, it->y()*32, (it->x()+4)*32, (it->y()+3)*32, Colors::Yellow);
		Broodwar->drawTextMap(it->x()*32+2, it->y()*32+2, "%d", i++);
	}
	i = 0;
	for (list<TilePosition>::const_iterator it = tech.pos.begin();
		it != tech.pos.end(); ++it)
	{
		Broodwar->drawBoxMap(it->x()*32, it->y()*32, (it->x()+3)*32, (it->y()+2)*32, Colors::Yellow);
		Broodwar->drawTextMap(it->x()*32+2, it->y()*32+2, "%d", i++);
	}
	i = 0;
	for (list<TilePosition>::const_iterator it = cannons.pos.begin();
		it != cannons.pos.end(); ++it)
	{
		Broodwar->drawBoxMap(it->x()*32, it->y()*32, (it->x()+2)*32, (it->y()+2)*32, Colors::Yellow);
		Broodwar->drawTextMap(it->x()*32+2, it->y()*32+2, "%d", i++);
	}
}
#endif

void SimCityBuildingPlacer::onUnitDestroy(Unit* unit)
{
	if (unit->getPlayer() == Broodwar->self() && unit->getType().isBuilding())
	{
		UnitType ut = unit->getType();
		TilePosition tp = unit->getTilePosition();
		set<Unit*> closeBuildings = Broodwar->getUnitsInRadius(Position(tp), 8*32);

		/// TODO should check path
		/// add it back
		TheReservedMap->freeTiles(tp, ut.tileWidth(), ut.tileHeight());
		if (ut == UnitTypes::Protoss_Pylon)
		{
			set<Unit*> unpoweredBuildings = checkPower(closeBuildings);
			if (unpoweredBuildings.size())
				TheBuilder->build(ut, tp);
			else
				pylons.addAsSecondPos(tp); // not to wait too much
		}
		else 
			getPositionAccountant(ut)->addPos(tp);

		/// set rally points in order not to block units (in case it's an inner building)
		set<Unit*> unpoweredBuildings = checkPower(closeBuildings);
		if (unpoweredBuildings.size())
			powerBuildings(unpoweredBuildings);
		for (set<Unit*>::iterator it = closeBuildings.begin();
			it != closeBuildings.end(); ++it)
		{
			if (ut == UnitTypes::Protoss_Gateway || ut == UnitTypes::Protoss_Robotics_Facility)
			{
				Vec tmp((*it)->getTilePosition().x() - tp.x(), (*it)->getTilePosition().y() - tp.y());

				/// precautions w.r.t. a pylon in the middle of gates
				if (tmp.y > 0 && tmp.y < 6)
				{
					tmp.y += 3;
					tmp.y = max(tmp.y, 7);
				}
				else if (tmp.y < 0)
					tmp.y -= 1;
				if (tmp.x > 0 && tmp.x < 7)
				{
					tmp.x += 4;
					tmp.y = max(tmp.y, 8);
				}
				else if (tmp.x < 0)
					tmp.x -= 1;

				tmp.translate(Vec(tp.x(), tp.y()));
				if (tmp.toTilePosition().isValid())
					(*it)->setRallyPoint(Position(tmp.toTilePosition()));
			}
		}
	}
}

bool SimCityBuildingPlacer::canBuildHere(BWAPI::Unit* builder, BWAPI::TilePosition position, BWAPI::UnitType type) const
{
	// TODO FIXIT perhaps chance Broodwar->canBuildHere by Broodwar->isBuildable(_, true)
	if (type.isAddon()) type=type.whatBuilds().first;
	//returns true if we can build this type of unit here. Takes into account reserved tiles.
	if (!BWAPI::Broodwar->canBuildHere(builder, position, type))
		return false;
	for(int x = position.x(); x < position.x() + type.tileWidth(); x++)
		for(int y = position.y(); y < position.y() + type.tileHeight(); y++)
			if (TheReservedMap->isReserved(x,y))
				return false;
	return true;
}

bool SimCityBuildingPlacer::canBuildHereWithPath(BWAPI::Unit* builder, BWAPI::TilePosition position, BWAPI::UnitType type) const
{
	if (!canBuildHere(builder, position, type))
		return false;
	TilePosition tpChokeMid(frontChoke->getCenter());
	if (!MapManager::Instance().isBTWalkable(tpChokeMid))
		tpChokeMid = MapManager::Instance().closestWalkable(tpChokeMid, home->getRegion());
	return floodFillWalkable(position, type, tpChokeMid);
}

bool SimCityBuildingPlacer::canBuildHereWithSpace(BWAPI::Unit* builder, BWAPI::TilePosition position, BWAPI::UnitType type, int buildDist) const
{
	if (type.isAddon()) type=type.whatBuilds().first;
	if (!canBuildHere(builder, position, type))
		return false;
	if (type.isRefinery())
		return true;
	int startx = position.x() - buildDist;
	int endx = position.x() + type.tileWidth() + buildDist;
	int starty = position.y() - buildDist;
	int endy = position.y() + type.tileHeight() + buildDist;
	if (startx < 0 || starty < 0 || endx > BWAPI::Broodwar->mapWidth() || endy > BWAPI::Broodwar->mapHeight())
		return false;
	for(int x = startx; x < endx; x++)
		for(int y = starty; y < endy; y++)
			if (!BWAPI::Broodwar->isBuildable(x, y, true) || TheReservedMap->isReserved(x,y))
				return false;
	return true;
}

bool SimCityBuildingPlacer::fullCanBuildHere(BWAPI::Unit* builder, BWAPI::TilePosition position, BWAPI::UnitType type) const
{
	if (!canBuildHere(builder, position, type))
		return false;
	unsigned int maxx = position.x() + type.tileWidth();
	unsigned int maxy = position.y() + type.tileHeight();
	for (unsigned int x = position.x(); x < maxx; ++x)
		for (unsigned int y = position.y(); y < maxy; ++y)
		{
			for (set<Unit*>::const_iterator it = Broodwar->getUnitsOnTile(x, y).begin();
				it != Broodwar->getUnitsOnTile(x, y).end(); ++it)
			{
				if (!(*it)->getType().canMove())
					return false;
			}
		}
	return true;
}	

bool SimCityBuildingPlacer::blockedBySomething(BWAPI::TilePosition position, BWAPI::UnitType type)
{
	unsigned int maxx = position.x() + type.tileWidth();
	unsigned int maxy = position.y() + type.tileHeight();
	for (unsigned int x = position.x(); x < maxx; ++x)
		for (unsigned int y = position.y(); y < maxy; ++y)
		{
			for (set<Unit*>::const_iterator it = Broodwar->getUnitsOnTile(x, y).begin();
				it != Broodwar->getUnitsOnTile(x, y).end(); ++it)
			{
				if (!(*it)->getType().canMove())
					return true;
			}
		}
	return false;
}

/***
 * Gives a buildable TilePosition for the UnitType ut near seed if specified
 */
TilePosition SimCityBuildingPlacer::getTilePosition(const UnitType& ut,
													TilePosition seed)
{
	if (pylons.pos.size() < 2 || gates.pos.size() < 2 || tech.pos.size() < 2)
		generate();
	if (pylons.pos.size() && !TheBasesManager->getRegionsBases().count(BWTA::getRegion(pylons.pos.front())))
		generatePylonsPos();
	if (seed == TilePositions::None)
	{
		if (ut == UnitTypes::Protoss_Pylon)
		{
			pylons.cleanUp(); // CAN'T BE DONE IN reservePos() (crashes ensues, see uses)
			if (!pylons.pos.size())
				generate();
			return pylons.reservePos();
		}
		else if (ut == UnitTypes::Protoss_Gateway || ut == UnitTypes::Protoss_Stargate)
		{
			gates.cleanUp();
			if (!gates.pos.size())
				generate();
			return gates.reservePos();
		}
		else if (ut == UnitTypes::Protoss_Photon_Cannon)
		{
			cannons.cleanUp();
			if (!cannons.pos.size())
				generateCannonsPos();
			return cannons.reservePos();
		}
		else
		{
			tech.cleanUp();
			if (!tech.pos.size())
				generate();
			return tech.reservePos();
		}
		return TilePositions::None; // def
	}
	else
	{
		if (ut == UnitTypes::Protoss_Pylon)
			return pylons.findClosest(seed);
		else if (ut == UnitTypes::Protoss_Gateway || ut == UnitTypes::Protoss_Stargate)
		    return gates.findClosest(seed);
		else if (ut == UnitTypes::Protoss_Photon_Cannon)
			return cannons.findClosest(seed);
		else
			return tech.findClosest(seed);
	}
}

void SimCityBuildingPlacer::releaseTilePosition(const TilePosition& tp, const UnitType& ut)
{
	if (ut == UnitTypes::Protoss_Pylon)
		pylons.freePos(tp);
	else if (ut == UnitTypes::Protoss_Gateway || ut == UnitTypes::Protoss_Stargate)
		gates.freePos(tp);
	else if (ut == UnitTypes::Protoss_Photon_Cannon)
		cannons.freePos(tp);
	else
		tech.freePos(tp);
}

void SimCityBuildingPlacer::usedTilePosition(const TilePosition& tp, const UnitType& ut)
{
	if (ut == UnitTypes::Protoss_Pylon)
		pylons.usedPos(tp);
	else if (ut == UnitTypes::Protoss_Gateway || ut == UnitTypes::Protoss_Stargate)
		gates.usedPos(tp);
	else if (ut == UnitTypes::Protoss_Photon_Cannon)
		cannons.usedPos(tp);
	else
		tech.usedPos(tp);
}

TilePosition SimCityBuildingPlacer::getPylonTilePositionCovering(const TilePosition& tp)
{
	TilePosition ret(TilePositions::None);
	for each (TilePosition t in pylons.pos)
	{
		TilePosition tt(t.x()+1, t.y()+1);
		if (tt.getDistance(TilePosition(tp.x()+1, tp.y()+1)) <= __PYLON_COVERAGE_TILES__*TILE_SIZE // TODO wrong, should be tp.x()+TYPE.tileWidth()/2, same for y...
			&& pylons.reservePos(t) != TilePositions::None)
			return t;
	}
	/// stupid (but robust) heuristic
	TilePosition t(TilePositions::None);
	TilePosition test1(TilePosition(tp.x() - 2, tp.y()));
	TilePosition test2(TilePosition(tp.x() + 2, tp.y()));
	TilePosition test3(TilePosition(tp.x(), tp.y() - 2));
	TilePosition test4(TilePosition(tp.x(), tp.y() + 2));
	if (canBuildHere(NULL, test1, UnitTypes::Protoss_Pylon)
		&& canBuildHere(NULL, TilePosition(test1.x()-1, test1.y()-1), UnitTypes::Protoss_Pylon)
		&& canBuildHere(NULL, TilePosition(test1.x()-1, test1.y()+1), UnitTypes::Protoss_Pylon)
		&& canBuildHere(NULL, TilePosition(test1.x(), test1.y()-1), UnitTypes::Protoss_Pylon)
		&& canBuildHere(NULL, TilePosition(test1.x(), test1.y()+1), UnitTypes::Protoss_Pylon))
		t = test1;
	else if (canBuildHere(NULL, test2, UnitTypes::Protoss_Pylon)
		&& canBuildHere(NULL, TilePosition(test2.x()+1, test2.y()-1), UnitTypes::Protoss_Pylon)
		&& canBuildHere(NULL, TilePosition(test2.x()+1, test2.y()+1), UnitTypes::Protoss_Pylon)
		&& canBuildHere(NULL, TilePosition(test2.x(), test2.y()-1), UnitTypes::Protoss_Pylon)
		&& canBuildHere(NULL, TilePosition(test2.x(), test2.y()+1), UnitTypes::Protoss_Pylon))
		t = test2;
	else if (canBuildHere(NULL, test3, UnitTypes::Protoss_Pylon)
		&& canBuildHere(NULL, TilePosition(test3.x()-1, test3.y()-1), UnitTypes::Protoss_Pylon)
		&& canBuildHere(NULL, TilePosition(test3.x()-1, test3.y()), UnitTypes::Protoss_Pylon)
		&& canBuildHere(NULL, TilePosition(test3.x()+1, test3.y()-1), UnitTypes::Protoss_Pylon)
		&& canBuildHere(NULL, TilePosition(test3.x()+1, test3.y()), UnitTypes::Protoss_Pylon))
		t = test3;
	else if (canBuildHere(NULL, test4, UnitTypes::Protoss_Pylon)
		&& canBuildHere(NULL, TilePosition(test4.x()-1, test4.y()), UnitTypes::Protoss_Pylon)
		&& canBuildHere(NULL, TilePosition(test4.x()-1, test4.y()+1), UnitTypes::Protoss_Pylon)
		&& canBuildHere(NULL, TilePosition(test4.x()+1, test4.y()), UnitTypes::Protoss_Pylon)
		&& canBuildHere(NULL, TilePosition(test4.x()+1, test4.y()+1), UnitTypes::Protoss_Pylon))
		t = test4;
	if (t != TilePositions::None)
	{
		pylons.addPos(t);
		ret = pylons.reservePos(t);
	}
	return ret;
}

TilePosition SimCityBuildingPlacer::generateBuildLocationNear(const TilePosition& tp, const BWAPI::UnitType& ut, int buildDist)
{
	/// returns a valid build location (with buildDist space around) near/around the TilePosition tp
	multimap<double, TilePosition> openTiles;
	openTiles.insert(make_pair(0, tp));
	pair<TilePosition, TilePosition> outerBounds = getOuterBoundsRegion(BWTA::getRegion(tp));
	for (int x = outerBounds.first.x(); x < outerBounds.second.x(); ++x)
		for (int y = outerBounds.first.y(); y < outerBounds.second.y(); ++y)
		{
			TilePosition tmp(x, y);
			if (Broodwar->isBuildable(tmp, true))
			{
				openTiles.insert(make_pair(tp.getDistance(tmp), tmp));
			}
		}
	for each (pair<double, TilePosition> pp in openTiles)
    {
		TilePosition t = pp.second;
		if (canBuildHereWithSpace(NULL, t, ut, buildDist))
			return t;
    }
	return TilePositions::None;
}