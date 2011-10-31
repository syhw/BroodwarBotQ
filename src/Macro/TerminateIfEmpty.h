#pragma once
#include "Macro/MacroManager/TaskStreamObserver.h"
#include "Macro/MacroManager/TaskStream.h"
#include <BWAPI.h>
class TerminateIfEmpty : public TaskStreamObserver
{
  public:
    TerminateIfEmpty();
    virtual void newStatus(TaskStream* ts);
};
