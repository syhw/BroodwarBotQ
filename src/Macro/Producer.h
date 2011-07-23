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
	BWAPI::Unit* operator->() const;
	void train(BWAPI::UnitType t);
};

class Producer : public Arbitrator::Controller<BWAPI::Unit*, double>
{
public:
	static Producer* create();
	static void destroy();
	inline bool checkCanProduce(BWAPI::UnitType t);
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
	std::multimap<int, BWAPI::UnitType> _productionQueue;
	Producer();
	~Producer();
};

extern Producer* TheProducer;