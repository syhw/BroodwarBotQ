#pragma once
#include <MacroManager/TaskStreamObserver.h>
#include <MacroManager/TaskStream.h>
#include <BWAPI.h>
class TerminateIfWorkerLost : public TaskStreamObserver
{
  public:
    TerminateIfWorkerLost();
    virtual void newStatus(TaskStream* ts);
};
