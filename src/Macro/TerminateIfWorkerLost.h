#pragma once
#include "Macro/MacroManager/TaskStreamObserver.h"
#include "Macro/MacroManager/TaskStream.h"
#include <BWAPI.h>
class TerminateIfWorkerLost : public TaskStreamObserver
{
  public:
    TerminateIfWorkerLost();
    virtual void newStatus(TaskStream* ts);
};
