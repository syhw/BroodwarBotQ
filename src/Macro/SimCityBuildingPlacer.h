#pragma once
#include "Macro/MacroManager/TaskStreamObserver.h"
#include "Macro/MacroManager/TaskStream.h"
#include "Macro/RectangleArray.h"
#include <list>
#include <set>

struct PositionAccountant
{
	std::list<BWAPI::TilePosition> pos;
	std::set<BWAPI::TilePosition> givenPos;
	BWAPI::TilePosition reservePos(Task& task);
	BWAPI::TilePosition reservePos();
	inline void freePos(BWAPI::TilePosition tp)
	{
		givenPos.erase(tp);
	}
	inline void usedPos(BWAPI::TilePosition tp)
	{
		pos.remove(tp);
		givenPos.erase(tp);
	}
	inline bool empty() const
	{
		return pos.empty();
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
	BWAPI::TilePosition homeRegionCenter;
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
	inline int canBuildCluster(const BWAPI::TilePosition& center, bool vertical);
	inline void makeCluster(const BWAPI::TilePosition& center,
		int nbTechBuildings, bool vertical);
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
