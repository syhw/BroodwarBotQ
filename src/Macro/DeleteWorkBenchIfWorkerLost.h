#pragma once
#include <Macro/MacroManager/TaskStreamObserver.h>
#include <Macro/MacroManager/TaskStream.h>
#include <BWAPI.h>
class DeleteWorkBenchIfWorkerLost : public TaskStreamObserver
{
  public:
    DeleteWorkBenchIfWorkerLost();
    virtual void onFrame(TaskStream* ts);
};
