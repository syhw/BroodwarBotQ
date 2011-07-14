#pragma once
#include "Macro/MacroManager/TaskStreamObserver.h"
#include "Macro/MacroManager/TaskStream.h"
#include "Macro/RectangleArray.h"
#include "Macro/ReservedMap.h"
#include <list>
#include <set>
#include "Defines.h"

struct BuildingsCluster
{
	BWAPI::TilePosition center;
	int size;
	bool vertical;
};

struct PositionAccountant
{
	BWAPI::UnitType ut;
	std::list<BWAPI::TilePosition> pos;
	std::set<BWAPI::TilePosition> givenPos;
	BWAPI::TilePosition reservePos(Task& task);
	BWAPI::TilePosition reservePos();
	inline void freePos(const BWAPI::TilePosition& tp)
	{
		givenPos.erase(tp);
	}
	inline void usedPos(const BWAPI::TilePosition& tp)
	{
		pos.remove(tp);
		givenPos.erase(tp);
	}
	inline bool empty() const
	{
		return pos.empty();
	}
	inline void reservePos(const BWAPI::TilePosition& tp)
	{
		pos.push_back(tp);
		TheReservedMap->reserveTiles(tp, ut, ut.tileWidth(), ut.tileHeight());
	}
	PositionAccountant(BWAPI::UnitType type)
		: ut(type)
	{
	}
};

class SimCityBuildingPlacer : public TaskStreamObserver
{
public:
	static SimCityBuildingPlacer* getInstance();
	SimCityBuildingPlacer();
	virtual void attached(TaskStream* ts);
	virtual void detached(TaskStream* ts);
	virtual void newStatus(TaskStream* ts);
	virtual void completedTask(TaskStream* ts, const Task &t);
	virtual void update(TaskStream* ts);
	void setTilePosition(TaskStream* ts, BWAPI::TilePosition p);
	void setRelocatable(TaskStream* ts, bool isRelocatable);
	void setBuildDistance(TaskStream* ts, int distance);
private:
	std::list<BWAPI::TilePosition> existingPylons;
	PositionAccountant pylons;
	PositionAccountant gates;
	PositionAccountant cannons;
	PositionAccountant tech;
	BWTA::BaseLocation* home;
	BWTA::Chokepoint* frontChoke;
	std::set<BWTA::Chokepoint*> backdoorChokes;
	int nbClusters;

	BWAPI::TilePosition buildFartherFrom(const BWAPI::TilePosition& tp,
		const BWAPI::TilePosition& fartherFrom,
		int howMuchFarther,
		const BWAPI::UnitType& ut);
	BWAPI::TilePosition getBuildLocationNear(BWAPI::Unit* builder,
		BWAPI::TilePosition position,
		BWAPI::UnitType type, int buildDist);
    inline BuildingsCluster searchForCluster(BWTA::Region* r);
	inline int canBuildCluster(const BWAPI::TilePosition& center, bool vertical);
	inline int makeCluster(const BWAPI::TilePosition& center,
		int nbTechBuildings, bool vertical, int cSize=0);
    inline void makeCannonsMinerals(BWTA::BaseLocation* home);
	inline void generate();
	bool canBuildHere(BWAPI::Unit* builder, BWAPI::TilePosition position, BWAPI::UnitType type) const;
	bool canBuildHereWithSpace(BWAPI::Unit* builder, BWAPI::TilePosition position, BWAPI::UnitType type, int buildDist) const;
	bool buildable(BWAPI::Unit* builder, int x, int y) const;
	struct data
	{
		bool isRelocatable;
		int buildDistance;
		BWAPI::TilePosition reservePosition;
		int reserveWidth;
		int reserveHeight;
	};
	std::map< TaskStream*, data > taskStreams;

};
