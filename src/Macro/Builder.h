#pragma once
#include "Macro/Arbitrator.h"
#include "Macro/SimCityBuildingPlacer.h"
#include <BWAPI.h>
#include <list>

class Task : public Arbitrator::Controller<BWAPI::Unit*, double>
{
	BWAPI::Unit* worker;
	BWAPI::TilePosition tilePosition;
	BWAPI::UnitType type;
	int lastOrder;
	bool finished;
public:
	static SimCityBuildingPlacer* buildingPlacer;
	Task(BWAPI::Unit* w, BWAPI::TilePosition tp, BWAPI::UnitType ut);
	virtual void onOffer(std::set<BWAPI::Unit*> units);
	virtual void onRevoke(BWAPI::Unit* unit, double bid);
	inline void askWorker();
	inline void buildIt();
	virtual std::string getName() const;
	virtual void update();
	bool isFinished();
};

class Builder
{
public:
	static Builder* create();
	static void destroy();
	void build(BWAPI::UnitType t, BWAPI::TilePosition seedPosition=BWAPI::TilePositions::None);
	void buildOrder(BWAPI::UnitType t, int supplyAsTime, BWAPI::TilePosition seedPosition=BWAPI::TilePositions::None);
	void update();
private:
	SimCityBuildingPlacer buildingPlacer;
	std::list<Task> tasks;
	std::multimap<int, Task> boTasks;
	Builder();
	~Builder();
};
extern Builder* TheBuilder;