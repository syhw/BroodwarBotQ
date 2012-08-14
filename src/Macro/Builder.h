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
	int tries;
	bool positioned;
	bool powered;
	bool reserved;
	bool finished;
public:
	static SimCityBuildingPlacer* buildingPlacer;
	static int framesToCompleteRequirements(BWAPI::UnitType type);
	Task(BWAPI::Unit* w, BWAPI::TilePosition tp=BWAPI::TilePositions::None, BWAPI::UnitType ut=BWAPI::UnitTypes::None, int lo=0);
	inline bool requirements();
	inline void positionIt();
	~Task();
	virtual void onOffer(std::set<BWAPI::Unit*> units);
	virtual void onRevoke(BWAPI::Unit* unit, double bid);
	inline void askWorker();
	inline void buildIt();
	inline void powerIt();
	BWAPI::TilePosition getTilePosition() const;
	virtual std::string getName() const;
	virtual std::string getShortName() const;
	void check();
	virtual void update();
	const BWAPI::UnitType& getType() const;
	int getLastOrder() const;
	bool hasReserved();
	bool isFinished();
};

class Builder
{
public:
	static Builder* create();
	static void destroy();
	void addTask(const BWAPI::UnitType& t, const BWAPI::TilePosition& seedPosition, bool quick, int lastOrder=0);
	void build(const BWAPI::UnitType& t, const BWAPI::TilePosition& seedPosition=BWAPI::TilePositions::None, bool quick=false);
	void buildOrder(const BWAPI::UnitType& t, int supplyAsTime, const BWAPI::TilePosition& seedPosition=BWAPI::TilePositions::None);
	void buildCannonsMinerals(BWTA::BaseLocation* b);
	int numberInFutureTasks(const BWAPI::UnitType& t);
	int numberInConstruction(const BWAPI::UnitType& t);
	int willBuild(const BWAPI::UnitType& t);      // Dumb helper functions that should be replaced by a better datastructure
	const std::list<BWAPI::Unit*>& getInConstruction();
	std::list<BWAPI::Unit*> getInConstruction(const BWAPI::UnitType& t);
	std::list<BWAPI::TilePosition> getFutureTaskPos(const BWAPI::UnitType& t, Task* before);
	int additionalSupplyNextFrames(int frames=30*24); // UnitTypes::Protoss_Pylon.buildTime()
	void update();
	const BWAPI::UnitType& nextBuildingType();
	void onUnitCreate(BWAPI::Unit* unit);
	void onUnitDestroy(BWAPI::Unit* unit);
private:
	SimCityBuildingPlacer buildingPlacer;
	std::list<pTask> tasks;
	std::list<BWAPI::Unit*> inConstruction;
	Builder();
	~Builder();
};
extern Builder* TheBuilder;