#pragma once
#include "Macro/Arbitrator.h"
#include <BWAPI.h>
#include <set>
#include <map>

struct ProducingUnit
{
	int lastAction;
	BWAPI::Unit* unit;
	ProducingUnit(BWAPI::Unit* u);
	explicit ProducingUnit(const ProducingUnit& pu);
	BWAPI::Unit* operator->() const;
	bool train(BWAPI::UnitType t);
};

class Producer : public Arbitrator::Controller<BWAPI::Unit*, double>
{
public:
	static Producer* create();
	static void destroy();
	inline bool checkCanProduce(BWAPI::UnitType t);
	int willProduce(BWAPI::UnitType t);
	void produce(int number, BWAPI::UnitType t, int priority=20, int increment=1);
	void produceAdditional(int number, BWAPI::UnitType t, int priority=20, int increment=1);
	void update();

	void onUnitCreate(BWAPI::Unit* unit);
	void onUnitDestroy(BWAPI::Unit* unit);

	virtual void onOffer(std::set<BWAPI::Unit*> units);
	virtual void onRevoke(BWAPI::Unit* unit, double bid);
	virtual std::string getName() const;
private:
	std::multimap<BWAPI::UnitType, BWAPI::Unit*> _techStructures;
	std::multimap<BWAPI::UnitType, ProducingUnit> _producingStructures;
	std::list<BWAPI::Unit*> _techStructuresInConstruction;
	std::list<BWAPI::Unit*> _producingStructuresInConstruction;
	std::multimap<int, BWAPI::UnitType> _productionQueue;
	std::set<BWAPI::UnitType> _neededProductionBuildings;
	int additionalUnitsSupply(int frames=30*24); // UnitTypes::Protoss_Pylon.buildTime()
	const BWAPI::UnitType& mostSaturatedUT();
	Producer();
	~Producer();

	inline void addToProducingStructures(BWAPI::Unit* u);
	inline void mergeArchons();
	inline void freeMerging();
    int _nbArchons;
	int _nbDarkArchons;
	std::map<BWAPI::Unit*, int> _htsMerging;
	std::map<BWAPI::Unit*, int> _dtsMerging;
	std::list<BWAPI::Unit*> _hts;
	std::list<BWAPI::Unit*> _dts;
};

extern Producer* TheProducer;