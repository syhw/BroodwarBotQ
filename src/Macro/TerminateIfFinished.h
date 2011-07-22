#pragma once
#include <Macro/MacroManager/TaskStreamObserver.h>
#include <Macro/MacroManager/TaskStream.h>
#include <BWAPI.h>
class TerminateIfFinished : public TaskStreamObserver
{
  public:
    TerminateIfFinished();
    virtual void onCompletedTask(TaskStream* ts);
};
