#pragma once
#include "Macro/Arbitrator.h"
#include "Macro/SimCityBuildingPlacer.h"
#include <BWAPI.h>
#include <list>
#include <boost/shared_ptr.hpp>

class Task;
typedef boost::shared_ptr<Task> pTask;

class Task : public Arbitrator::Controller<BWAPI::Unit*, double>
{
	BWAPI::Unit* worker;
	BWAPI::TilePosition tilePosition;
	BWAPI::UnitType type;
	int lastOrder;
	bool finished;
public:
	static SimCityBuildingPlacer* buildingPlacer;
	Task(BWAPI::Unit* w, BWAPI::TilePosition tp, BWAPI::UnitType ut, int lo=0);
	void init();
	~Task();
	virtual void onOffer(std::set<BWAPI::Unit*> units);
	virtual void onRevoke(BWAPI::Unit* unit, double bid);
	inline void askWorker();
	inline void buildIt();
	virtual std::string getName() const;
	virtual std::string getShortName() const;
	virtual void update();
	const BWAPI::UnitType& getType();
	bool isFinished();
};

class Builder
{
public:
	static Builder* create();
	static void destroy();
	void addTask(BWAPI::UnitType t, BWAPI::TilePosition seedPosition, int lastOrder=0);
	void build(BWAPI::UnitType t, BWAPI::TilePosition seedPosition=BWAPI::TilePositions::None);
	void buildOrder(BWAPI::UnitType t, int supplyAsTime, BWAPI::TilePosition seedPosition=BWAPI::TilePositions::None);
	bool willBuild(BWAPI::UnitType t);
	void update();
	const BWAPI::UnitType& nextBuildingType();
	void onUnitCreate(BWAPI::Unit* unit);
	void onUnitDestroy(BWAPI::Unit* unit);
private:
	SimCityBuildingPlacer buildingPlacer;
	std::list<pTask> tasks;
	std::multimap<int, pTask> boTasks;
	std::list<BWAPI::Unit*> inConstruction;
	Builder();
	~Builder();
};
extern Builder* TheBuilder;