#pragma once
#include "Macro/Arbitrator.h"
#include "Macro/SimCityBuildingPlacer.h"
#include <BWAPI.h>
#include <list>
#include <boost/shared_ptr.hpp>

struct pl_unit
{
	const BWAPI::UnitType& type;
	int prio;
	BWAPI::TilePosition tilepos;
	pl_unit(const BWAPI::UnitType& ut, int pr, BWAPI::TilePosition tp)
		: type(ut), prio(pr), tilepos(tp)
	{}
};

class Planner
{
	Planner();
	~Planner();
	std::list<pl_unit> requests;
	std::list<pl_unit> plan;
	std::multimap<int, const BWAPI::UnitType&> build_order;
public:
	void buildOrder(const BWAPI::UnitType& ut, int supplyAsTime);
	std::pair<const BWAPI::UnitType&, BWAPI::TilePosition> getNextBuilding();
	const BWAPI::UnitType& getNextUnit();
	void add(const BWAPI::UnitType& ut, int number=1, int prio=50, 
		BWAPI::TilePosition tp=BWAPI::TilePositions::None);
	void insert_plan(const BWAPI::UnitType& ut, int prio=50);
	void update();
};

extern Planner* ThePlanner;