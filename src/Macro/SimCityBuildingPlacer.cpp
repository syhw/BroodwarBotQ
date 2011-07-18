#include <PrecompiledHeader.h>
#include "Macro/SimCityBuildingPlacer.h"
#include "Macro/Heap.h"
#include "Utils/Vec.h"
#include "Regions/MapManager.h"
#include "Macro/BasesManager.h"

using namespace std;
using namespace BWAPI;

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
			if (mm->isBTWalkable(tmp) && !occupied.count(tmp))
				tmp1.insert(tmp);
			tmp = TilePosition(it->x(), it->y()+1);
			if (mm->isBTWalkable(tmp) && !occupied.count(tmp))
				tmp1.insert(tmp);
			tmp = TilePosition(it->x()-1, it->y());
			if (mm->isBTWalkable(tmp) && !occupied.count(tmp))
				tmp1.insert(tmp);
			tmp = TilePosition(it->x(), it->y()-1);
			if (mm->isBTWalkable(tmp) && !occupied.count(tmp))
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
			if (mm->isBTWalkable(tmp) && !occupied.count(tmp))
				tmp2.insert(tmp);
			tmp = TilePosition(it->x(), it->y()+1);
			if (from1.count(tmp))
				return true;
			if (mm->isBTWalkable(tmp) && !occupied.count(tmp))
				tmp2.insert(tmp);
			tmp = TilePosition(it->x()-1, it->y());
			if (from1.count(tmp))
				return true;
			if (mm->isBTWalkable(tmp) && !occupied.count(tmp))
				tmp2.insert(tmp);
			tmp = TilePosition(it->x(), it->y()-1);
			if (from1.count(tmp))
				return true;
			if (mm->isBTWalkable(tmp) && !occupied.count(tmp))
				tmp2.insert(tmp);
		}
		tmp2.swap(from2);
	}
	return false;
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

void SimCityBuildingPlacer::generate()
{
	BuildingsCluster bc;
	if (nbClusters < 3)
	{
		bc = searchForCluster(home->getRegion());
	}
	if (!bc.size)
	{
		for (set<Base*>::const_iterator it = TheBasesManager->getActiveBases().begin();
			it != TheBasesManager->getActiveBases().end(); ++it)
		{
			if ((*it)->getBaseLocation()->getRegion() != home->getRegion())
				bc = searchForCluster((*it)->getBaseLocation()->getRegion());
			if (bc.size)
				break;
		}
	}
	if (!bc.center.isValid())
		bc.center.makeValid();
	if (bc.size)
		makeCluster(bc.center, 2, bc.vertical, bc.size);
	else
		Broodwar->printf("Could not generate a cluster");
}

BuildingsCluster SimCityBuildingPlacer::searchForCluster(BWTA::Region* r)
{
	BuildingsCluster ret;
	ret.size = 0;
    int minX = 100000000;
	int minY = 100000000;
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
	/// search for "big clusters"
	int minXClusterDim = UnitTypes::Protoss_Pylon.tileWidth() + 2*UnitTypes::Protoss_Gateway.tileWidth() + 2; // 2 to move around
	int minYClusterDim = UnitTypes::Protoss_Pylon.tileHeight() + 2*UnitTypes::Protoss_Gateway.tileHeight() + 2;
	int tmpMaxX = min(maxX - minXClusterDim, Broodwar->mapWidth());
	int tmpMaxY = min(maxY - minYClusterDim, Broodwar->mapHeight());
	for (int x = minX; x < tmpMaxX; x += 1)
		for (int y = minY; y < tmpMaxY; y += 1)
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
			if (fullCanBuildHere(NULL, topLeft, UnitTypes::Protoss_Pylon)
				&& fullCanBuildHere(NULL, TilePosition(topRight.x() - UnitTypes::Protoss_Pylon.tileWidth(), topRight.y()), UnitTypes::Protoss_Pylon)
				&& fullCanBuildHere(NULL, TilePosition(botLeft.x(), botLeft.y() - UnitTypes::Protoss_Pylon.tileHeight()), UnitTypes::Protoss_Pylon)
				&& fullCanBuildHere(NULL, TilePosition(botRight.x() - UnitTypes::Protoss_Pylon.tileWidth(), botRight.y() - UnitTypes::Protoss_Pylon.tileHeight()), UnitTypes::Protoss_Pylon))
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
	tmpMaxX = min(maxX - minXClusterDim, Broodwar->mapWidth());
	tmpMaxY = min(maxY - minYClusterDim, Broodwar->mapHeight());
	for (int x = minX; x < tmpMaxX; x += 1)
		for (int y = minY; y < tmpMaxY; y += 1)
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
			if (fullCanBuildHere(NULL, topLeft, UnitTypes::Protoss_Pylon)
				&& fullCanBuildHere(NULL, TilePosition(topRight.x() - UnitTypes::Protoss_Pylon.tileWidth(), topRight.y()), UnitTypes::Protoss_Pylon)
				&& fullCanBuildHere(NULL, TilePosition(botLeft.x(), botLeft.y() - UnitTypes::Protoss_Pylon.tileHeight()), UnitTypes::Protoss_Pylon)
				&& fullCanBuildHere(NULL, TilePosition(botRight.x() - UnitTypes::Protoss_Pylon.tileWidth(), botRight.y() - UnitTypes::Protoss_Pylon.tileHeight()), UnitTypes::Protoss_Pylon))
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
	tmpMaxX = min(maxX - minXClusterDim, Broodwar->mapWidth());
	tmpMaxY = min(maxY - minYClusterDim, Broodwar->mapHeight());
	for (int x = minX; x < tmpMaxX; x += 1)
		for (int y = minY; y < tmpMaxY; y += 1)
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
			if (fullCanBuildHere(NULL, topLeft, UnitTypes::Protoss_Pylon)
				&& fullCanBuildHere(NULL, TilePosition(topRight.x() - UnitTypes::Protoss_Pylon.tileWidth(), topRight.y()), UnitTypes::Protoss_Pylon)
				&& fullCanBuildHere(NULL, TilePosition(botLeft.x(), botLeft.y() - UnitTypes::Protoss_Pylon.tileHeight()), UnitTypes::Protoss_Pylon)
				&& fullCanBuildHere(NULL, TilePosition(botRight.x() - UnitTypes::Protoss_Pylon.tileWidth(), botRight.y() - UnitTypes::Protoss_Pylon.tileHeight()), UnitTypes::Protoss_Pylon))
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

/***
 * Returns either false (0) or the size (1=small, 2=big) of the cluster
 * one can build at around "center"
 */
int SimCityBuildingPlacer::canBuildCluster(const TilePosition& center, bool vertical)
{
	int clusterSize = 3;
	if (vertical)
	{
		int minX = center.x() - UnitTypes::Protoss_Gateway.tileWidth() - 1; // -1 additional tile to move around
		if (minX < 0)
			return 0;
		int maxX = center.x() + UnitTypes::Protoss_Pylon.tileWidth() + UnitTypes::Protoss_Gateway.tileWidth();
		if (maxX >= Broodwar->mapWidth())
			return 0;
		// 1 stage of buildings (2 gates or 1 gate 1 tech or 2 tech buildings)
		int minY1 = center.y() - UnitTypes::Protoss_Pylon.tileHeight() - 1;
		if (minY1 < 0)
			return 0;
		int maxY1 = center.y() + UnitTypes::Protoss_Pylon.tileHeight();
		if (maxY1 >= Broodwar->mapHeight())
			return 0;
		int minY = minY1;
		int maxY = maxY1;
		// 2 stages
		int minY2 = center.y() - UnitTypes::Protoss_Pylon.tileHeight() - 1;
		int maxY2 = center.y() + 2*UnitTypes::Protoss_Pylon.tileHeight();
		// 3 stages
		int minY3 = center.y() - 2*UnitTypes::Protoss_Pylon.tileHeight() - 1;
		int maxY3 = center.y() + 3*UnitTypes::Protoss_Pylon.tileHeight();
		if (minY2 < 0 || maxY2 >= Broodwar->mapHeight())
			clusterSize = 1;
		else
		{
			minY = minY2;
			maxY = maxY2;
			if (minY3 < 0 || maxY3 >= Broodwar->mapHeight())
				clusterSize = 2;
			else
			{
				minY = minY3;
				maxY = maxY3;
			}
		}
		for (int x = minX; x <= maxX; ++x)
			for (int y = minY; y <= maxY; ++y)
			{
				if (!fullCanBuildHere(NULL, TilePosition(x, y), UnitTypes::Protoss_Pylon))
				{
					if (y > maxY2 || y < minY2)
						clusterSize = 2;
					else if (y > maxY1 || y < minY1)
						clusterSize = 1;
					else
						return 0;
				}
			}
		return clusterSize;
	}
	else // horizontal
	{
		int minY = center.y() - UnitTypes::Protoss_Gateway.tileHeight() - 1; // -1 additional tile to move around
		if (minY < 0)
			return 0;
		int maxY = center.y() + UnitTypes::Protoss_Pylon.tileHeight() + UnitTypes::Protoss_Gateway.tileHeight();
		if (maxY >= Broodwar->mapHeight())
			return 0;
		// 1 stage of buildings (2 gates or 1 gate 1 tech or 2 tech buildings)
		int minX1 = center.x() - UnitTypes::Protoss_Pylon.tileWidth() - 1;
		int maxX1 = center.x() + UnitTypes::Protoss_Pylon.tileWidth();
		if (minX1 < 0)
			return 0;
		if (maxX1 >= Broodwar->mapWidth())
			return 0;
		int minX = minX1;
		int maxX = maxX1;
		// 2 stages
		int minX2 = center.x() - 2*UnitTypes::Protoss_Pylon.tileWidth() - 1;
		int maxX2 = center.x() + 2*UnitTypes::Protoss_Pylon.tileWidth();
		// 3 stages
		int minX3 = center.x() - 3*UnitTypes::Protoss_Pylon.tileWidth() - 1;
		int maxX3 = center.x() + 3*UnitTypes::Protoss_Pylon.tileWidth();
		if (minX2 < 0 || maxX2 >= Broodwar->mapWidth())
			clusterSize = 1;
		else
		{
			minX = minX2;
			maxX = maxX2;
			if (minX3 < 0 || maxX3 >= Broodwar->mapWidth())
				clusterSize = 2;
			else
			{
				minX = minX3;
				maxX = maxX3;
			}
		}
		for (int x = minX3; x <= maxX3; ++x)
			for (int y = minY; y <= maxY; ++y)
			{
				if (!fullCanBuildHere(NULL, TilePosition(x, y), UnitTypes::Protoss_Pylon))
				{
					if (x > maxX2 || x < minX2)
						clusterSize = 2;
					else if (x > maxX1 || x < minX1)
						clusterSize = 1;
					else
						return 0;
				}
			}
		return clusterSize;
	}
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
	if (clusterSize > 3)
	{
		Broodwar->printf("WHAT THE FUCK???");
	}
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

void SimCityBuildingPlacer::makeCannonsMinerals(BWTA::BaseLocation* hom)
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
	dist = 1000000.0;
	double distb = 1000000.0;
	double dist2 = 1000000.0;
	double dist2b = 1000000.0;
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
			if (tmp.getDistance(hom->getTilePosition()) < 6)
				continue;
			double tmpd1 = tmp.getDistance(furtherMineral);
			if (tmpd1 < dist && fullCanBuildHere(NULL, tmp, UnitTypes::Protoss_Pylon))
			{
				pylon1 = cannon1;
				cannon1 = tmp;
				dist = tmpd1;
			}
			else if (tmpd1 < distb && fullCanBuildHere(NULL, tmp, UnitTypes::Protoss_Pylon))
			{
				pylon1 = tmp;
				distb = tmpd1;
			}
			double tmpd2 = tmp.getDistance(gasTilePos);
			if (tmpd2 < dist2 && fullCanBuildHere(NULL, tmp, UnitTypes::Protoss_Pylon))
			{
				pylon2 = cannon2;
				cannon2 = tmp;
				dist2 = tmpd2;
			}
			else if (tmpd2 < dist2b && fullCanBuildHere(NULL, tmp, UnitTypes::Protoss_Pylon))
			{
				pylon2 = tmp;
				dist2b = tmpd2;
			}
		}
	cannons.addPos(cannon1);
	pylons.addPos(pylon1);
	cannons.addPos(cannon2);
	pylons.addPos(pylon2);
}

SimCityBuildingPlacer::SimCityBuildingPlacer()
: home(BWTA::getStartLocation(Broodwar->self()))
, nbClusters(0)
, pylons(UnitTypes::Protoss_Pylon)
, gates(UnitTypes::Protoss_Gateway)
, cannons(UnitTypes::Protoss_Photon_Cannon)
, tech(UnitTypes::Protoss_Cybernetics_Core)
{
	/// search and save front and backdoor chokes
	backdoorChokes = home->getRegion()->getChokepoints();
	double minDist = 100000000.0;
	for (set<BWTA::Chokepoint*>::const_iterator it = backdoorChokes.begin();
		it != backdoorChokes.end(); ++it)
	{
		double tmpDist = BWTA::getGroundDistance(TilePosition((*it)->getCenter()), 
			TilePosition(Broodwar->mapHeight()/2, Broodwar->mapWidth()/2));
		if (tmpDist < minDist)
		{
			minDist = tmpDist;
			frontChoke = *it;
		}
	}
	backdoorChokes.erase(frontChoke);

	TilePosition nexus = home->getTilePosition();
	TilePosition front = TilePosition(frontChoke->getCenter());

	/// best place to do a pylons/gates cluster
	Vec dir(front.x() - nexus.x(), front.y() - nexus.y());
	bool vertical = abs((int)dir.y) > abs((int)dir.x);
	Vec nex(nexus.x(), nexus.y());
	dir = dir.normalize();
	dir *= 2*UnitTypes::Protoss_Gateway.tileWidth() + UnitTypes::Protoss_Pylon.tileWidth() + 1;
	nex = nex.vecTranslate(dir);
	TilePosition cluster_center(nex.toTilePosition());
	if (!makeCluster(cluster_center, 1, vertical))
		generate();

	/// search places to put cannons behind minerals
	makeCannonsMinerals(home);

	/// search places to put cannons at chokes
	// TODO
}

void SimCityBuildingPlacer::update()
{
#ifdef __DEBUG__
	for (list<TilePosition>::const_iterator it = pylons.pos.begin();
		it != pylons.pos.end(); ++it)
		Broodwar->drawBoxMap(it->x()*32, it->y()*32, (it->x()+2)*32, (it->y()+2)*32, Colors::Yellow);
	for (list<TilePosition>::const_iterator it = gates.pos.begin();
		it != gates.pos.end(); ++it)
		Broodwar->drawBoxMap(it->x()*32, it->y()*32, (it->x()+4)*32, (it->y()+3)*32, Colors::Yellow);
	for (list<TilePosition>::const_iterator it = tech.pos.begin();
		it != tech.pos.end(); ++it)
		Broodwar->drawBoxMap(it->x()*32, it->y()*32, (it->x()+3)*32, (it->y()+2)*32, Colors::Yellow);
	for (list<TilePosition>::const_iterator it = cannons.pos.begin();
		it != cannons.pos.end(); ++it)
		Broodwar->drawBoxMap(it->x()*32, it->y()*32, (it->x()+2)*32, (it->y()+2)*32, Colors::Yellow);
#endif
	if (pylons.pos.size() < 2 || gates.pos.size() < 2 || tech.pos.size() < 2)
		generate();
}

bool SimCityBuildingPlacer::canBuildHere(BWAPI::Unit* builder, BWAPI::TilePosition position, BWAPI::UnitType type) const
{
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

TilePosition SimCityBuildingPlacer::getTilePosition(const UnitType& ut)
{
	if (ut == UnitTypes::Protoss_Pylon)
	{
		if (!pylons.pos.size())
			generate();
		return pylons.reservePos();
	}
	else if (ut == UnitTypes::Protoss_Gateway || ut == UnitTypes::Protoss_Stargate)
	{
		if (!gates.pos.size())
			generate();
		return gates.reservePos();
	}
	else if (ut == UnitTypes::Protoss_Photon_Cannon)
	{
		if (!cannons.pos.size())
			generate();
		return cannons.reservePos();
	}
	else
	{
		if (!tech.pos.size())
			generate();
		return tech.reservePos();
	}
	return TilePositions::None; // def
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