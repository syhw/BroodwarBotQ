#pragma once
#include <BWAPI.h>
#include <map>
#include <Task.h>
namespace UnitReadyTimeStatus
{
  enum Enum
  {
    Error_Task_Requires_Addon,
    Waiting_For_Earliest_Start_Time,
    Waiting_For_Worker_To_Be_Ready,
    Waiting_For_Free_Time,
    Waiting_For_Required_Units,
    Waiting_For_Required_Tech,
    Waiting_For_Required_Upgrade,
    Waiting_For_Supply,
    Waiting_For_Gas,
    Waiting_For_Minerals,
    Error_Worker_Not_Specified,
    Error_Worker_Not_Owned
  };
};
class UnitReadyTimeCalculator
{
  public:
    static void init();
    /* Computes when a unit will be ready to start any task */
    static int getReadyTime(BWAPI::Unit* unit, bool considerTasks = false);
    /* Computes when a unit will be ready to start any task of a given duration */
    static int getReadyTime(BWAPI::Unit* unit, int duration);
    /* computes when a unit will be free to start a specific task */
    static int getFirstFreeTime(BWAPI::Unit* unit, const Task &task, UnitReadyTimeStatus::Enum &reason, bool considerResources = true, bool considerTasks = false);
};