#pragma once
#include <Macro/MacroManager/TaskStreamObserver.h>
#include <Macro/MacroManager/TaskStream.h>
#include <BWAPI.h>
class TerminateIfNoWorkBenches : public TaskStreamObserver
{
  public:
    TerminateIfNoWorkBenches();
    virtual void onFrame(TaskStream* ts);
};
