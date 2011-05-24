#pragma once
#include <MacroManager/TaskStreamObserver.h>
#include <MacroManager/TaskStream.h>
#include <BWAPI.h>
class TerminateIfEmpty : public TaskStreamObserver
{
  public:
    TerminateIfEmpty();
    virtual void newStatus(TaskStream* ts);
};
