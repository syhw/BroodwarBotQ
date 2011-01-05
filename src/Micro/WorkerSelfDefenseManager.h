#pragma once
#include <Arbitrator.h>
#include <BWAPI.h>
#include <UnitGroupManager.h>
#include <Heap.h>
class WorkerSelfDefenseManager : public Arbitrator::Controller<BWAPI::Unit*,double>
{
  public:
    WorkerSelfDefenseManager(Arbitrator::Arbitrator<BWAPI::Unit*,double>* arbitrator);
    virtual void onOffer(std::set<BWAPI::Unit*> units);
    virtual void onRevoke(BWAPI::Unit* unit, double bid);
    virtual void update();
    virtual std::string getName() const;
    virtual std::string getShortName() const;
    void onRemoveUnit(BWAPI::Unit* unit);
  private:
    Arbitrator::Arbitrator<BWAPI::Unit*,double>* arbitrator;
    std::set<BWAPI::Unit*> myWorkers;
    std::map<BWAPI::Unit*,Arbitrator::Heap<BWAPI::Unit*, int> > getAttackers;
};