#pragma once
#include "Macro/Arbitrator.h"
#include <BWAPI.h>
#include <set>
#include <map>

typedef std::pair<int, BWAPI::UnitType> ProductionOrder;

class Producer : public Arbitrator::Controller<BWAPI::Unit*, double>
{
public:
	static Producer* create();
	static void destroy();
	void produce(int number, BWAPI::UnitType t, int priority=20);
	void update();

	void onUnitCreate(BWAPI::Unit* unit);
	void onUnitDestroy(BWAPI::Unit* unit);

	virtual void onOffer(std::set<BWAPI::Unit*> units);
	virtual void onRevoke(BWAPI::Unit* unit, double bid);
	virtual std::string getName() const;
private:
	std::multimap<BWAPI::UnitType, BWAPI::Unit*> producingStructures;
	std::multimap<int, ProductionOrder> prioritizedOrders;
	Producer();
	~Producer();
};
extern Producer* TheProducer;