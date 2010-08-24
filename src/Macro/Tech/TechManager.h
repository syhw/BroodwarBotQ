#pragma once
#include <Arbitrator.h>
#include <BWAPI.h>
#include <BuildingPlacer.h>
#include "CSingleton.h"
class TechManager : public Arbitrator::Controller<BWAPI::Unit*,double>, public CSingleton<TechManager>
{
	friend class CSingleton<TechManager>;
  public:

	void setDependencies();
    virtual void onOffer(std::set<BWAPI::Unit*> units);
    virtual void onRevoke(BWAPI::Unit* unit, double bid);
    virtual void update();
    virtual std::string getName() const;
    void onRemoveUnit(BWAPI::Unit* unit);
    bool research(BWAPI::TechType type);
    bool planned(BWAPI::TechType type) const;
    Arbitrator::Arbitrator<BWAPI::Unit*,double>* arbitrator;
    BuildingPlacer* placer;
    std::map<BWAPI::UnitType,std::list<BWAPI::TechType> > researchQueues;
    std::map<BWAPI::Unit*,BWAPI::TechType> researchingUnits;
    std::set<BWAPI::TechType> plannedTech;
private:
	    TechManager();
};