#pragma once
#include <MacroManager/TaskStreamObserver.h>
#include <MacroManager/TaskStream.h>
#include <BWAPI.h>
class UnitPump : public TaskStreamObserver
{
  public:
    UnitPump(BWAPI::UnitType t);
    virtual void update(TaskStream* ts);
  private:
    BWAPI::UnitType type;
};
