#pragma once
#include <MacroManager/TaskStreamObserver.h>
#include <MacroManager/TaskStream.h>
#include <BWAPI.h>
class UnitCompositionProducer : public TaskStreamObserver
{
  public:
    UnitCompositionProducer(BWAPI::UnitType workerType);
    virtual void attached(TaskStream* ts);
    virtual void detached(TaskStream* ts);
    void update();
    void setUnitWeight(BWAPI::UnitType t, double weight);
    BWAPI::UnitType getNextUnitType(BWAPI::Unit* worker);
  private:
    std::set<TaskStream*> streams;
    BWAPI::UnitType workerType;
    std::map<BWAPI::UnitType, double> unitCompositionWeights;
    std::map<BWAPI::UnitType, int> actualUnitCounts;
    std::map<BWAPI::UnitType, double> desiredUnitCounts;
};
