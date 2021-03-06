#pragma once
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
	BuildingsCluster()
		: size(0)
	{}
};

struct PositionAccountant
{
	BWAPI::UnitType ut;
	std::list<BWAPI::TilePosition> pos;
	std::set<BWAPI::TilePosition> givenPos;
	void cleanUp(); // because maps suck?
	BWAPI::TilePosition reservePos();
	BWAPI::TilePosition reservePos(BWAPI::TilePosition tp);
	BWAPI::TilePosition findClosest(BWAPI::TilePosition seed);
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
	inline void addPos(const BWAPI::TilePosition& tp)
	{
		pos.push_back(tp);
		TheReservedMap->reserveTiles(tp, ut, ut.tileWidth(), ut.tileHeight());
	}
	inline void addAsSecondPos(const BWAPI::TilePosition& tp)
	{
		if (pos.empty())
			pos.push_front(tp);
		else
		{
			std::list<BWAPI::TilePosition>::const_iterator it = pos.begin();
			pos.insert(++it, tp); // put in second position
		}
		TheReservedMap->reserveTiles(tp, ut, ut.tileWidth(), ut.tileHeight());
	}
	PositionAccountant(BWAPI::UnitType type)
		: ut(type)
	{
	}
};

class SimCityBuildingPlacer
{
public:
	SimCityBuildingPlacer();
	BWAPI::TilePosition getTilePosition(const BWAPI::UnitType& ut, BWAPI::TilePosition seed = BWAPI::TilePositions::None);
	BWAPI::TilePosition getPylonTilePositionCovering(const BWAPI::TilePosition& tp);
	BWAPI::TilePosition generateBuildLocationNear(const BWAPI::TilePosition& tp, const BWAPI::UnitType& ut, int buildDist=1);
	void releaseTilePosition(const BWAPI::TilePosition& tp, const BWAPI::UnitType& ut);
	void usedTilePosition(const BWAPI::TilePosition& tp, const BWAPI::UnitType& ut);
#ifdef __MACRO_DEBUG__
	void update();
#endif
	void onUnitDestroy(BWAPI::Unit* unit);
    void makeCannonsMinerals(BWTA::BaseLocation* home, bool quick=false);
	void makeCannonChoke(BWTA::Region* inter, BWTA::Chokepoint* chok, bool quick=false);
	static bool blockedBySomething(BWAPI::TilePosition position, BWAPI::UnitType type);
	static bool inMineralLine(BWTA::BaseLocation* b, BWAPI::TilePosition tp);
private:
	std::list<BWAPI::TilePosition> existingPylons;
	PositionAccountant pylons;
	PositionAccountant gates;
	PositionAccountant cannons;
	PositionAccountant tech;
	PositionAccountant* getPositionAccountant(const BWAPI::UnitType& ut);
	BWTA::BaseLocation* home;
	BWTA::Chokepoint* frontChoke;
	std::set<BWTA::Chokepoint*> backdoorChokes;
	int nbClusters;
	bool _noMoreClustersAtHome;
	bool _canNoLongerGenerateClusters;
	std::set<BWTA::BaseLocation*> _searchedForClustersAtBase;
	std::set<BWTA::Region*> _searchedForClustersAtRegion;
	inline void searchedAtBase(BWTA::BaseLocation* b);

	inline BuildingsCluster searchForCluster(BWTA::BaseLocation* b);
    inline BuildingsCluster searchForCluster(BWTA::Region* r);
	inline BuildingsCluster searchForCluster(int minX, int maxX, int minY, int maxY, BWTA::Region* r);
	inline bool canBuildCluster(BWAPI::TilePosition center, bool vertical, int size);
	inline int canBuildCluster(const BWAPI::TilePosition& center, bool vertical);
	inline int makeCluster(const BWAPI::TilePosition& center,
		int nbTechBuildings, bool vertical, int cSize=0);
	BWAPI::TilePosition closestBuildableSameRegion(const BWAPI::TilePosition& tp);
	BWAPI::TilePosition closestBuildableSameRegionNotTP2(const BWAPI::TilePosition& tp, const BWAPI::TilePosition& tp2);
	inline void generate(int min_size = 1);
	inline void generateGatesPos();
	inline void generateTechPos();
	inline void generateCannonsPos();
	inline void generatePylonsPos();
	inline std::set<BWAPI::Unit*> checkPower(const std::set<BWAPI::Unit*>& buildings);
	inline bool powerBuildings(const std::set<BWAPI::Unit*>& buildings);
	bool canBuildHere(BWAPI::Unit* builder, BWAPI::TilePosition position, BWAPI::UnitType type) const;
	bool canBuildHereWithPath(BWAPI::Unit* builder, BWAPI::TilePosition position, BWAPI::UnitType type) const;
	bool canBuildHereWithSpace(BWAPI::Unit* builder, BWAPI::TilePosition position, BWAPI::UnitType type, int buildDist=1) const;
	bool fullCanBuildHere(BWAPI::Unit* builder, BWAPI::TilePosition position, BWAPI::UnitType type) const; // double checks (map init?)
};
