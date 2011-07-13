#include <PrecompiledHeader.h>
#include "Macro/SimCityBuildingPlacer.h"
#include "Macro/ReservedMap.h"
#include "Macro/Heap.h"
#include "Utils/Vec.h"
#include "Regions/MapManager.h"

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

/***
 * Gives a buildable TilePosition farther from nexus
 */
TilePosition SimCityBuildingPlacer::buildFartherFrom(const TilePosition& tp,
							  const TilePosition& fartherFrom,
							  int howMuchFarther,
							  const UnitType& ut)
{
	Vec dir(tp.x() - fartherFrom.x(), tp.y() - fartherFrom.y());
    int signx = 1;
	if (dir.x)
		signx = (int)(dir.x/abs(dir.x));
	TilePosition tmpx(tp.x() + signx*howMuchFarther, tp.y());
	if (abs(dir.x) > abs(dir.y) || canBuildHere(NULL, tmpx, ut))
		return tmpx;
	int signy = 1;
	if (dir.y)
		signy = int(dir.y/abs(dir.y));
	TilePosition tmpy(tp.x(), tp.y() + signy*howMuchFarther);
	if (canBuildHere(NULL, tmpy, ut))
		return tmpy;
	if (canBuildHere(NULL, tmpx, ut))
		return tmpx;
	return TilePositions::None;
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

void PositionAccountant::generate()
{
}

SimCityBuildingPlacer* instance = NULL;
SimCityBuildingPlacer* SimCityBuildingPlacer::getInstance()
{
	if (instance==NULL)
		instance = new SimCityBuildingPlacer();
	return instance;
}

void SimCityBuildingPlacer::makeCluster(const TilePosition& center, int nbTechBuildings, bool vertical)
{
	int techToPlace = min(4, nbTechBuildings);
	unsigned int minX;
	unsigned int maxX;
	unsigned int minY;
	unsigned int maxY;
	if (vertical)
	{
		minX = center.x() - UnitTypes::Protoss_Gateway.tileWidth() - 1; // -1 additional tile to move around
		maxX = center.x() + UnitTypes::Protoss_Pylon.tileWidth()-1 + UnitTypes::Protoss_Gateway.tileWidth() + 1;
		minY = center.y() - 2*UnitTypes::Protoss_Pylon.tileHeight() - 1;
		maxY = center.y() + 2*UnitTypes::Protoss_Pylon.tileHeight() + 2;
	}
	else // horizontal
	{
		minX = center.x() - 2*UnitTypes::Protoss_Pylon.tileWidth() - 1; // -1 additional tile to move around
		maxX = center.x() + 2*UnitTypes::Protoss_Pylon.tileWidth() + 2;
		minY = center.y() - UnitTypes::Protoss_Gateway.tileHeight() - 1;
		maxY = center.y() + UnitTypes::Protoss_Pylon.tileHeight()-1 + UnitTypes::Protoss_Gateway.tileHeight() + 1;
	}
	bool canBuildCluster = true;
	bool canBuildSmallCluster = true;
	for (unsigned int x = minX; x < maxX; ++x)
	{
		for (unsigned int y = minY; y < maxY; ++y)
		{
			if (!canBuildHere(NULL, TilePosition(x, y), UnitTypes::Protoss_Pylon))
			{
				canBuildCluster = false;
				if (x > minX+2 && x < maxX-2 && y > minY+2 && y < maxY-2)
				{
					canBuildSmallCluster = false;
					break;
				}
			}
		}
		if (!canBuildSmallCluster)
			break;
	}
	int techPlaced = 0;
	if (canBuildSmallCluster)
	{
		if (vertical)
		{
			pylons.pos.push_back(TilePosition(center.x(), center.y()));
			pylons.pos.push_back(TilePosition(center.x(), center.y() - UnitTypes::Protoss_Pylon.tileHeight()));
			pylons.pos.push_back(TilePosition(center.x(), center.y() + UnitTypes::Protoss_Pylon.tileHeight()));
			int shift = 1;
			if (canBuildCluster)
			{
				pylons.pos.push_back(TilePosition(center.x(), center.y() - 2*UnitTypes::Protoss_Pylon.tileHeight()));
				pylons.pos.push_back(TilePosition(center.x(), center.y() + 2*UnitTypes::Protoss_Pylon.tileHeight()));
				shift = -1;
				gates.pos.push_back(TilePosition(center.x() - UnitTypes::Protoss_Gateway.tileWidth(), center.y() + shift + UnitTypes::Protoss_Gateway.tileHeight()));
				gates.pos.push_back(TilePosition(center.x() + UnitTypes::Protoss_Pylon.tileWidth(), center.y() + shift + UnitTypes::Protoss_Gateway.tileHeight()));
			}
			if (techPlaced < techToPlace)
			{
				tech.pos.push_back(TilePosition(center.x() + UnitTypes::Protoss_Pylon.tileWidth(), center.y() + shift));
				++techPlaced;
			}
			else
				gates.pos.push_back(TilePosition(center.x() + UnitTypes::Protoss_Pylon.tileWidth(), center.y() + shift));
			if (techPlaced < techToPlace)
			{
				tech.pos.push_back(TilePosition(center.x() - UnitTypes::Protoss_Cybernetics_Core.tileWidth(), center.y() + shift));
				++techPlaced;
			}
			else
				gates.pos.push_back(TilePosition(center.x() - UnitTypes::Protoss_Gateway.tileWidth(), center.y() + shift));
			if (techPlaced < techToPlace)
			{
				tech.pos.push_back(TilePosition(center.x() + UnitTypes::Protoss_Pylon.tileWidth(), center.y() + shift - UnitTypes::Protoss_Gateway.tileHeight()));
				++techPlaced;
			}
			else
				gates.pos.push_back(TilePosition(center.x() + UnitTypes::Protoss_Pylon.tileWidth(), center.y() + shift - UnitTypes::Protoss_Gateway.tileHeight()));
			if (techPlaced < techToPlace)
			{
				tech.pos.push_back(TilePosition(center.x() - UnitTypes::Protoss_Cybernetics_Core.tileWidth(), center.y() + shift - UnitTypes::Protoss_Gateway.tileHeight()));
				++techPlaced;
			}
			else
				gates.pos.push_back(TilePosition(center.x() - UnitTypes::Protoss_Gateway.tileWidth(), center.y() + shift - UnitTypes::Protoss_Gateway.tileHeight()));
		}
		else
		{
			pylons.pos.push_back(TilePosition(center.x(), center.y()));
			pylons.pos.push_back(TilePosition(center.x() + UnitTypes::Protoss_Pylon.tileWidth(), center.y()));
			pylons.pos.push_back(TilePosition(center.x() - UnitTypes::Protoss_Pylon.tileWidth(), center.y()));
			int shift = 1;
			if (canBuildCluster)
				shift = 0;
			if (techPlaced < techToPlace)
			{
				tech.pos.push_back(TilePosition(center.x() + shift - UnitTypes::Protoss_Gateway.tileWidth(), center.y() - UnitTypes::Protoss_Cybernetics_Core.tileHeight()));
				++techPlaced;
			}
			else
				gates.pos.push_back(TilePosition(center.x() + shift - UnitTypes::Protoss_Gateway.tileWidth(), center.y() - UnitTypes::Protoss_Gateway.tileHeight()));
			if (techPlaced < techToPlace)
			{
				++techPlaced;
			}
			else
				gates.pos.push_back(TilePosition(center.x() + shift - UnitTypes::Protoss_Gateway.tileWidth(), center.y() + UnitTypes::Protoss_Pylon.tileHeight()));
			if (techPlaced < techToPlace)
			{
				++techPlaced;
			}
			else
				gates.pos.push_back(TilePosition(center.x() + shift, center.y() - UnitTypes::Protoss_Gateway.tileHeight()));
			if (techPlaced < techToPlace)
			{
				++techPlaced;
			}
			else
				gates.pos.push_back(TilePosition(center.x() + shift, center.y() + UnitTypes::Protoss_Pylon.tileHeight()));
			if (canBuildCluster)
			{
				gates.pos.push_back(TilePosition(center.x() + UnitTypes::Protoss_Gateway.tileWidth(), center.y() - UnitTypes::Protoss_Gateway.tileHeight()));
				gates.pos.push_back(TilePosition(center.x() + UnitTypes::Protoss_Gateway.tileWidth(), center.y() + UnitTypes::Protoss_Pylon.tileHeight()));
				pylons.pos.push_back(TilePosition(center.x() - 2*UnitTypes::Protoss_Pylon.tileWidth(), center.y()));
				pylons.pos.push_back(TilePosition(center.x() + 2*UnitTypes::Protoss_Pylon.tileWidth(), center.y()));
			}
		}
	}
}

void SimCityBuildingPlacer::makeCannonsMinerals(BWTA::BaseLocation* home)
{
	int x = 0;
	int y = 0;
	for (set<Unit*>::const_iterator it = home->getMinerals().begin();
		it != home->getMinerals().end(); ++it)
	{
		x += (*it)->getTilePosition().x();
		y += (*it)->getTilePosition().y();
	}
	TilePosition gasTilePos;
	for (set<Unit*>::const_iterator it = home->getGeysers().begin();
		it != home->getGeysers().end(); ++it)
	{
		gasTilePos = (*it)->getTilePosition();
		x += gasTilePos.x();
		y += gasTilePos.y();
	}
	TilePosition meanMineral(x / (home->getMinerals().size()+home->getGeysers().size()),
		y / (home->getMinerals().size()+home->getGeysers().size()));
	double dist = 0.0;
	TilePosition furtherMineral;
	for (set<Unit*>::const_iterator it = home->getMinerals().begin();
		it != home->getMinerals().end(); ++it)
	{
		double tmp = meanMineral.getDistance((*it)->getTilePosition());
		if (tmp > dist)
		{
			dist = tmp;
			furtherMineral = (*it)->getTilePosition();
		}
	}
	TilePosition tmp = buildFartherFrom(furtherMineral, home->getTilePosition(), 3, UnitTypes::Protoss_Photon_Cannon);
	cannons.pos.push_back(tmp);
	pylons.pos.push_back(buildFartherFrom(tmp, home->getTilePosition(), 3, UnitTypes::Protoss_Pylon));
    tmp = buildFartherFrom(gasTilePos, home->getTilePosition(), 4, UnitTypes::Protoss_Photon_Cannon);
	cannons.pos.push_back(tmp);
	pylons.pos.push_back(buildFartherFrom(tmp, home->getTilePosition(), 3, UnitTypes::Protoss_Pylon));
}

SimCityBuildingPlacer::SimCityBuildingPlacer()
: home(BWTA::getStartLocation(Broodwar->self()))
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

	/// search the center of our home Region
	homeRegionCenter = TilePosition(home->getRegion()->getCenter()); // region->getCenter() is bad (can be out of the Region)
	BWTA::Polygon polygon = home->getRegion()->getPolygon();
	set<TilePosition> out;
	list<TilePosition>in;
	for (vector<Position>::const_iterator it = polygon.begin();
		it != polygon.end(); ++it)
	{
		in.push_back(TilePosition(*it));
	}
	while (!out.empty())
	{
		if (out.size() == 1)
		{
			homeRegionCenter = *(out.begin());
			break;
		}
		else
		{
			list<TilePosition> newIn;
			for (list<TilePosition>::const_iterator it = in.begin();
				it != in.end(); ++it)
			{
				out.erase(*it);
				newIn.push_back(TilePosition(it->x()+1, it->y()));
				newIn.push_back(TilePosition(it->x(), it->y()+1));
				newIn.push_back(TilePosition(it->x()-1, it->y()));
				newIn.push_back(TilePosition(it->x(), it->y()-1));
			}
			in.swap(newIn);
		}
	}
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
	makeCluster(cluster_center, 1, vertical);

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
			// GENERATE
		}
	}
	else if (type == UnitTypes::Protoss_Gateway || type == UnitTypes::Protoss_Stargate)
	{
		if (!gates.empty())
			return gates.reservePos();
		else
		{
			// GENERATE
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
			// GENERATE
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
