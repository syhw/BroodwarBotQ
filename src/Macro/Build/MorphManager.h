#pragma once
#include <CSingleton.h>
#include <Arbitrator.h>
#include <BWAPI.h>
class MorphManager : public Arbitrator::Controller<BWAPI::Unit*,double>, public CSingleton<MorphManager>
{
	friend class CSingleton<MorphManager>;

	private:
		MorphManager();
		~MorphManager();

  public:
    class Unit
    {
      public:
        BWAPI::UnitType type;
        bool started;
    };
    virtual void onOffer(std::set<BWAPI::Unit*> units);
    virtual void onRevoke(BWAPI::Unit* unit, double bid);
    virtual void update();
    virtual std::string getName() const;

    void onUnitDestroy(BWAPI::Unit* unit);
    bool morph(BWAPI::UnitType type);
    int getPlannedCount(BWAPI::UnitType type) const;
    int getStartedCount(BWAPI::UnitType type) const;

  private:
    Arbitrator::Arbitrator<BWAPI::Unit*,double>* arbitrator;
    std::map<BWAPI::UnitType,std::list<BWAPI::UnitType> > morphQueues;
    std::map<BWAPI::Unit*,Unit> morphingUnits;
    std::map<BWAPI::UnitType, int> plannedCount;
    std::map<BWAPI::UnitType, int> startedCount;
};