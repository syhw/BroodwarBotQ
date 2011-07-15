#include <PrecompiledHeader.h>
#include "Macro/SimCityBuildingPlacer.h"
#include "Macro/Heap.h"
#include "Utils/Vec.h"
#include "Regions/MapManager.h"
#include "MacroBaseManager.h"

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

TilePosition PositionAccountant::reservePos(Task& task)
{
	for (list<BWAPI::TilePosition>::const_iterator it = pos.begin();
		it != pos.end(); ++it)
	{
		if (!givenPos.count(*it))
		{
			givenPos.erase(task.getTilePosition());
			givenPos.insert(*it);
			task.setTilePosition(*it);
			return *it;
		}
	}
	return TilePositions::None;
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
		for (set<MacroBase*>::const_iterator it = TheMacroBaseManager->getActiveBases().begin();
			it != TheMacroBaseManager->getActiveBases().end(); ++it)
		{
			if ((*it)->getBaseLocation()->getRegion() != home->getRegion())
				bc = searchForCluster((*it)->getBaseLocation()->getRegion());
			if (bc.size)
				break;
		}
	}
	if (bc.size)
		makeCluster(bc.center, 2, bc.vertical, bc.size);
}

SimCityBuildingPlacer* instance = NULL;
SimCityBuildingPlacer* SimCityBuildingPlacer::getInstance()
{
	if (instance==NULL)
		instance = new SimCityBuildingPlacer();
	return instance;
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
			if (canBuildHere(NULL, topLeft, UnitTypes::Protoss_Gateway)
				&& canBuildHere(NULL, TilePosition(topRight.x() - UnitTypes::Protoss_Gateway.tileWidth(), topRight.y()), UnitTypes::Protoss_Gateway)
				&& canBuildHere(NULL, TilePosition(botLeft.x(), botLeft.y() - UnitTypes::Protoss_Gateway.tileHeight()), UnitTypes::Protoss_Gateway)
				&& canBuildHere(NULL, TilePosition(botRight.x() - UnitTypes::Protoss_Gateway.tileWidth(), botRight.y() - UnitTypes::Protoss_Gateway.tileHeight()), UnitTypes::Protoss_Gateway))
			{
				ret.center = TilePosition(topLeft.x() + UnitTypes::Protoss_Gateway.tileWidth() + 1, topLeft.y() + 2*UnitTypes::Protoss_Pylon.tileHeight() + 1);
				ret.vertical = true;
				ret.size = canBuildCluster(ret.center, ret.vertical);
				if (ret.size > 1)
					return ret;
				ret.center = TilePosition(topLeft.x() + 2*UnitTypes::Protoss_Pylon.tileWidth() + 1, topLeft.y() + UnitTypes::Protoss_Gateway.tileHeight() + 1);
				ret.vertical = false;
				ret.size = canBuildCluster(ret.center, ret.vertical);
				if (ret.size > 1)
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
			if (canBuildHere(NULL, topLeft, UnitTypes::Protoss_Gateway)
				&& canBuildHere(NULL, TilePosition(topRight.x() - UnitTypes::Protoss_Gateway.tileWidth(), topRight.y()), UnitTypes::Protoss_Gateway)
				&& canBuildHere(NULL, TilePosition(botLeft.x(), botLeft.y() - UnitTypes::Protoss_Gateway.tileHeight()), UnitTypes::Protoss_Gateway)
				&& canBuildHere(NULL, TilePosition(botRight.x() - UnitTypes::Protoss_Gateway.tileWidth(), botRight.y() - UnitTypes::Protoss_Gateway.tileHeight()), UnitTypes::Protoss_Gateway))
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
	unsigned int minX;
	unsigned int maxX;
	unsigned int minY;
	unsigned int maxY;
	if (vertical)
	{
		minX = center.x() - UnitTypes::Protoss_Gateway.tileWidth() - 1; // -1 additional tile to move around
		maxX = center.x() + UnitTypes::Protoss_Pylon.tileWidth() + UnitTypes::Protoss_Gateway.tileWidth() + 1;
		minY = center.y() - 2*UnitTypes::Protoss_Pylon.tileHeight() - 1;
		maxY = center.y() + 2*UnitTypes::Protoss_Pylon.tileHeight() + 2;
	}
	else // horizontal
	{
		minX = center.x() - 2*UnitTypes::Protoss_Pylon.tileWidth() - 1; // -1 additional tile to move around
		maxX = center.x() + 2*UnitTypes::Protoss_Pylon.tileWidth() + 2;
		minY = center.y() - UnitTypes::Protoss_Gateway.tileHeight() - 1;
		maxY = center.y() + UnitTypes::Protoss_Pylon.tileHeight() + UnitTypes::Protoss_Gateway.tileHeight() + 1;
	}
	bool canBuildCluster = true;
	for (unsigned int x = minX; x < maxX; ++x)
	{
		for (unsigned int y = minY; y < maxY; ++y)
		{
			if (!canBuildHere(NULL, TilePosition(x, y), UnitTypes::Protoss_Pylon))
			{
				canBuildCluster = false;
				if (x > minX+2 && x < maxX-2 && y > minY+2 && y < maxY-2)
					return 0;
			}
		}
	}
	if (canBuildCluster)
		return 2; // big cluster
	else
		return 1; // small cluster
}

int SimCityBuildingPlacer::makeCluster(const TilePosition& center, int nbTechBuildings, bool vertical, int cSize)
{
	++nbClusters;
	int clusterSize = 0;
	if (!cSize)
		clusterSize = canBuildCluster(center, vertical);
	else
		clusterSize = cSize;
	bool canBuildCluster = false;
	bool canBuildSmallCluster = false;
	if (clusterSize > 0)
		canBuildSmallCluster = true;
	if (clusterSize > 1)
		canBuildCluster = true;
	int techToPlace = min(4, nbTechBuildings);
	int techPlaced = 0;
	if (canBuildSmallCluster)
	{
		if (vertical)
		{
			pylons.reservePos(TilePosition(center.x(), center.y()));
			pylons.reservePos(TilePosition(center.x(), center.y() - UnitTypes::Protoss_Pylon.tileHeight()));
			pylons.reservePos(TilePosition(center.x(), center.y() + UnitTypes::Protoss_Pylon.tileHeight()));
			int shift = 1;
			if (canBuildCluster)
			{
				pylons.reservePos(TilePosition(center.x(), center.y() - 2*UnitTypes::Protoss_Pylon.tileHeight()));
				pylons.reservePos(TilePosition(center.x(), center.y() + 2*UnitTypes::Protoss_Pylon.tileHeight()));
				shift = -1;
				gates.reservePos(TilePosition(center.x() - UnitTypes::Protoss_Gateway.tileWidth(), center.y() + shift + UnitTypes::Protoss_Gateway.tileHeight()));
				gates.reservePos(TilePosition(center.x() + UnitTypes::Protoss_Pylon.tileWidth(), center.y() + shift + UnitTypes::Protoss_Gateway.tileHeight()));
			}
			if (techPlaced < techToPlace)
			{
				tech.reservePos(TilePosition(center.x() + UnitTypes::Protoss_Pylon.tileWidth(), center.y() + shift));
				++techPlaced;
			}
			else
				gates.reservePos(TilePosition(center.x() + UnitTypes::Protoss_Pylon.tileWidth(), center.y() + shift));
			if (techPlaced < techToPlace)
			{
				tech.reservePos(TilePosition(center.x() - UnitTypes::Protoss_Cybernetics_Core.tileWidth(), center.y() + shift));
				++techPlaced;
			}
			else
				gates.reservePos(TilePosition(center.x() - UnitTypes::Protoss_Gateway.tileWidth(), center.y() + shift));
			if (techPlaced < techToPlace)
			{
				tech.reservePos(TilePosition(center.x() + UnitTypes::Protoss_Pylon.tileWidth(), center.y() + shift - UnitTypes::Protoss_Gateway.tileHeight()));
				++techPlaced;
			}
			else
				gates.reservePos(TilePosition(center.x() + UnitTypes::Protoss_Pylon.tileWidth(), center.y() + shift - UnitTypes::Protoss_Gateway.tileHeight()));
			if (techPlaced < techToPlace)
			{
				tech.reservePos(TilePosition(center.x() - UnitTypes::Protoss_Cybernetics_Core.tileWidth(), center.y() + shift - UnitTypes::Protoss_Gateway.tileHeight()));
				++techPlaced;
			}
			else
				gates.reservePos(TilePosition(center.x() - UnitTypes::Protoss_Gateway.tileWidth(), center.y() + shift - UnitTypes::Protoss_Gateway.tileHeight()));
		}
		else
		{
			pylons.reservePos(TilePosition(center.x(), center.y()));
			pylons.reservePos(TilePosition(center.x() + UnitTypes::Protoss_Pylon.tileWidth(), center.y()));
			pylons.reservePos(TilePosition(center.x() - UnitTypes::Protoss_Pylon.tileWidth(), center.y()));
			int shift = 1;
			if (canBuildCluster)
				shift = 0;
			if (techPlaced < techToPlace)
			{
				tech.reservePos(TilePosition(center.x() + shift - UnitTypes::Protoss_Gateway.tileWidth(), center.y() - UnitTypes::Protoss_Cybernetics_Core.tileHeight()));
				++techPlaced;
			}
			else
				gates.reservePos(TilePosition(center.x() + shift - UnitTypes::Protoss_Gateway.tileWidth(), center.y() - UnitTypes::Protoss_Gateway.tileHeight()));
			if (techPlaced < techToPlace)
			{
				++techPlaced;
			}
			else
				gates.reservePos(TilePosition(center.x() + shift - UnitTypes::Protoss_Gateway.tileWidth(), center.y() + UnitTypes::Protoss_Pylon.tileHeight()));
			if (techPlaced < techToPlace)
			{
				++techPlaced;
			}
			else
				gates.reservePos(TilePosition(center.x() + shift, center.y() - UnitTypes::Protoss_Gateway.tileHeight()));
			if (techPlaced < techToPlace)
			{
				++techPlaced;
			}
			else
				gates.reservePos(TilePosition(center.x() + shift, center.y() + UnitTypes::Protoss_Pylon.tileHeight()));
			if (canBuildCluster)
			{
				gates.reservePos(TilePosition(center.x() + UnitTypes::Protoss_Gateway.tileWidth(), center.y() - UnitTypes::Protoss_Gateway.tileHeight()));
				gates.reservePos(TilePosition(center.x() + UnitTypes::Protoss_Gateway.tileWidth(), center.y() + UnitTypes::Protoss_Pylon.tileHeight()));
				pylons.reservePos(TilePosition(center.x() - 2*UnitTypes::Protoss_Pylon.tileWidth(), center.y()));
				pylons.reservePos(TilePosition(center.x() + 2*UnitTypes::Protoss_Pylon.tileWidth(), center.y()));
			}
		}
	}
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
			if (tmpd1 < dist && canBuildHere(NULL, tmp, UnitTypes::Protoss_Pylon))
			{
				pylon1 = cannon1;
				cannon1 = tmp;
				dist = tmpd1;
			}
			else if (tmpd1 < distb && canBuildHere(NULL, tmp, UnitTypes::Protoss_Pylon))
			{
				pylon1 = tmp;
				distb = tmpd1;
			}
			double tmpd2 = tmp.getDistance(gasTilePos);
			if (tmpd2 < dist2 && canBuildHere(NULL, tmp, UnitTypes::Protoss_Pylon))
			{
				pylon2 = cannon2;
				cannon2 = tmp;
				dist2 = tmpd2;
			}
			else if (tmpd2 < dist2b && canBuildHere(NULL, tmp, UnitTypes::Protoss_Pylon))
			{
				pylon2 = tmp;
				dist2b = tmpd2;
			}
		}
	cannons.reservePos(cannon1);
	pylons.reservePos(pylon1);
	cannons.reservePos(cannon2);
	pylons.reservePos(pylon2);
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

void SimCityBuildingPlacer::attached(TaskStream* ts)
{
	if (ts->getTask(0).getTilePosition().isValid()==false)
		ts->getTask(0).setTilePosition(Broodwar->self()->getStartLocation());
	taskStreams[ts].isRelocatable   = true;
	taskStreams[ts].buildDistance   = 1;
	taskStreams[ts].reservePosition = ts->getTask(0).getTilePosition();
	taskStreams[ts].reserveWidth    = 0;
	taskStreams[ts].reserveHeight   = 0;
}

void SimCityBuildingPlacer::detached(TaskStream* ts)
{
	taskStreams.erase(ts);
}

void SimCityBuildingPlacer::newStatus(TaskStream* ts)
{
	// TODO cancel
	/*UnitType type = ts->getTask(0).getUnit();
	if (type == UnitTypes::Protoss_Pylon)
	{
		pylons.freePos(ts->getTask(0).getTilePosition());
	}
	else if (type == UnitTypes::Protoss_Gateway || type == UnitTypes::Protoss_Cybernetics_Core)
	{
		gates.freePos(ts->getTask(0).getTilePosition());
	}*/
}

void SimCityBuildingPlacer::completedTask(TaskStream* ts, const Task &t)
{
	TheReservedMap->freeTiles(taskStreams[ts].reservePosition,taskStreams[ts].reserveWidth,taskStreams[ts].reserveHeight);
	taskStreams[ts].reserveWidth  = 0;
	taskStreams[ts].reserveHeight = 0;
	if (t.getType() == TaskTypes::Unit && t.getUnit() == UnitTypes::Protoss_Pylon)
	{
		existingPylons.push_back(t.getTilePosition());
		pylons.usedPos(t.getTilePosition());
	}
	else if (t.getUnit() == UnitTypes::Protoss_Gateway || t.getUnit() == UnitTypes::Protoss_Stargate)
	{
		tech.usedPos(t.getTilePosition());
	}
	else if (t.getUnit() != UnitTypes::Protoss_Photon_Cannon)
	{
		gates.usedPos(t.getTilePosition());
	}
}

void SimCityBuildingPlacer::update(TaskStream* ts)
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
	
	if (Broodwar->getFrameCount()%10 != 0) return;

	if (ts->getTask(0).getType() != TaskTypes::Unit) return;

	UnitType type = ts->getTask(0).getUnit();
	std::string tmpS = type.getName();

	// don't look for a build location if this building requires power and we have no pylons
	if (type.requiresPsi() && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Pylon) == 0) return;
	// add-ons need their main building
	if (type.isAddon()) type = type.whatBuilds().first;

	int width = ts->getTask(0).getUnit().tileWidth();

	/// Something blocks construction
	if (ts->getStatus()==TaskStream::Error_Location_Blocked || ts->getStatus()==TaskStream::Error_Location_Not_Specified)
	{
		if (ts->getTask(0).getTilePosition().isValid()==false)
			ts->getTask(0).setTilePosition(Broodwar->self()->getStartLocation());
		/// Move it if we can
		if (taskStreams[ts].isRelocatable)
		{
			TilePosition tp(ts->getTask(0).getTilePosition());
			int bd = taskStreams[ts].buildDistance;
			TilePosition newtp = TilePositions::None;
			newtp = getBuildLocationNear(ts->getWorker(), tp, type, bd);
			if (type == UnitTypes::Protoss_Pylon)
				pylons.freePos(ts->getTask(0).getTilePosition());
			else if (type == UnitTypes::Protoss_Gateway || type == UnitTypes::Protoss_Stargate)
				gates.freePos(ts->getTask(0).getTilePosition());
			else if (type == UnitTypes::Protoss_Photon_Cannon)
				cannons.freePos(ts->getTask(0).getTilePosition());
			else
				tech.freePos(ts->getTask(0).getTilePosition());
			if (newtp != TilePositions::None)
			{
				ts->getTask(0).setTilePosition(newtp);
			}
		}
	}

	if (type==BWAPI::UnitTypes::Terran_Command_Center ||
		type==BWAPI::UnitTypes::Terran_Factory || 
		type==BWAPI::UnitTypes::Terran_Starport ||
		type==BWAPI::UnitTypes::Terran_Science_Facility)
	{
		width+=2;
	}

	if (taskStreams[ts].reserveWidth    != width ||
		taskStreams[ts].reserveHeight   != ts->getTask(0).getUnit().tileHeight() ||
		taskStreams[ts].reservePosition != ts->getTask(0).getTilePosition())
	{
		TheReservedMap->freeTiles(taskStreams[ts].reservePosition,taskStreams[ts].reserveWidth,taskStreams[ts].reserveHeight);
		taskStreams[ts].reserveWidth    = width;
		taskStreams[ts].reserveHeight   = ts->getTask(0).getUnit().tileHeight();
		taskStreams[ts].reservePosition = ts->getTask(0).getTilePosition();
		TheReservedMap->reserveTiles(taskStreams[ts].reservePosition,type,taskStreams[ts].reserveWidth,taskStreams[ts].reserveHeight);
	}
}

void SimCityBuildingPlacer::setTilePosition(TaskStream* ts, BWAPI::TilePosition p)
{
	ts->getTask(0).setTilePosition(p);
}

void SimCityBuildingPlacer::setRelocatable(TaskStream* ts, bool isRelocatable)
{
	taskStreams[ts].isRelocatable = isRelocatable;
}

void SimCityBuildingPlacer::setBuildDistance(TaskStream* ts, int distance)
{
	taskStreams[ts].buildDistance = distance;
}

BWAPI::TilePosition SimCityBuildingPlacer::getBuildLocationNear(BWAPI::Unit* builder, BWAPI::TilePosition position, BWAPI::UnitType type, int buildDist)
{
	/// Returns a valid build location near the specified tile position.
	std::string tmpS = type.getName();
	if (type.isAddon()) type = type.whatBuilds().first;

	if (type == UnitTypes::Protoss_Pylon)
	{
		if (!pylons.empty())
			return pylons.reservePos();
		else
		{
			generate();
		}
	}
	else if (type == UnitTypes::Protoss_Gateway || type == UnitTypes::Protoss_Stargate)
	{
		if (!gates.empty())
			return gates.reservePos();
		else
		{
			generate();
		}
	}
	else if (type.isRefinery())
	{
		set<Unit*>::const_iterator it = home->getGeysers().begin();
		if (canBuildHere(NULL, (*it)->getTilePosition(), type))
			return ((*it)->getTilePosition());
		else
		{
			// TODO other gas
		}
	}
	else if (type == UnitTypes::Protoss_Photon_Cannon)
	{
		if (!cannons.empty())
			return cannons.reservePos();
		else
		{
			// GENERATE
		}
	}
	else
	{
		if (!tech.empty())
			return tech.reservePos();
		else
		{
			generate();
		}
	}
	return BWAPI::TilePositions::None;
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

bool SimCityBuildingPlacer::canBuildHereWithSpace(BWAPI::Unit* builder, BWAPI::TilePosition position, BWAPI::UnitType type, int buildDist) const
{
	if (type.isAddon()) type=type.whatBuilds().first;
	//returns true if we can build this type of unit here with the specified amount of space.
	//space value is stored in this->buildDistance.

	//if we can't build here, we of course can't build here with space
	if (!this->canBuildHere(builder,position, type))
		return false;
	if (type.isRefinery())
		return true;

	int width=type.tileWidth();
	int height=type.tileHeight();

	//make sure we leave space for add-ons. These types of units can have addons:
	if (type==BWAPI::UnitTypes::Terran_Command_Center ||
		type==BWAPI::UnitTypes::Terran_Factory || 
		type==BWAPI::UnitTypes::Terran_Starport ||
		type==BWAPI::UnitTypes::Terran_Science_Facility)
	{
		width+=2;
	}
	int startx = position.x() - buildDist;
	if (startx<0) return false;
	int starty = position.y() - buildDist;
	if (starty<0) return false;
	int endx = position.x() + width + buildDist;
	if (endx>BWAPI::Broodwar->mapWidth()) return false;
	int endy = position.y() + height + buildDist;
	if (endy>BWAPI::Broodwar->mapHeight()) return false;

	for(int x = startx; x < endx; x++)
		for(int y = starty; y < endy; y++)
			if (!buildable(builder, x, y) || TheReservedMap->isReserved(x,y))
				return false;

	if (position.x()>3)
	{
		int startx2=startx-2;
		if (startx2<0) startx2=0;
		for(int x = startx2; x < startx; x++)
			for(int y = starty; y < endy; y++)
			{
				std::set<BWAPI::Unit*> units = BWAPI::Broodwar->getUnitsOnTile(x, y);
				for(std::set<BWAPI::Unit*>::iterator i = units.begin(); i != units.end(); i++)
				{
					if (!(*i)->isLifted() && *i != builder)
					{
						BWAPI::UnitType type=(*i)->getType();
						if (type==BWAPI::UnitTypes::Terran_Command_Center ||
							type==BWAPI::UnitTypes::Terran_Factory || 
							type==BWAPI::UnitTypes::Terran_Starport ||
							type==BWAPI::UnitTypes::Terran_Science_Facility)
						{
							return false;
						}
					}
				}
			}
	}
	return true;
}

bool SimCityBuildingPlacer::buildable(BWAPI::Unit* builder, int x, int y) const
{
	//returns true if this tile is currently buildable, takes into account units on tile
	if (!BWAPI::Broodwar->isBuildable(x,y)) return false;
	std::set<BWAPI::Unit*> units = BWAPI::Broodwar->getUnitsOnTile(x, y);
	for(std::set<BWAPI::Unit*>::iterator i = units.begin(); i != units.end(); i++)
		if ((*i)->getType().isBuilding() && !(*i)->isLifted() && !(*i)->getType().isFlyer() && *i != builder)
			return false;
	return true;
}
