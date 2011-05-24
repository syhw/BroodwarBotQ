#pragma once
#include <BWAPI.h>
#include <map>
#include <Task.h>


class WorkerTaskTimeline
{
  public:
    WorkerTaskTimeline();
    void reset();
    std::pair<int,int> getFirstFreeInterval(BWAPI::Unit* worker, int duration, int earliestStartTime = -1);
    std::pair<int,int> getFirstFreeInterval(BWAPI::Unit* worker, const Task* task, int earliestStartTime = -1);
    bool reserveTime(BWAPI::Unit* worker, int startFrame, Task* task);
    Task* getPlannedTask(BWAPI::Unit* worker, int frame);
  private:
    std::map<BWAPI::Unit*, std::map<int, Task*> > taskStartTimes;
};