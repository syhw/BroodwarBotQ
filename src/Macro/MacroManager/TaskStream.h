#pragma once
#include <Task.h>
#include <Arbitrator.h>
#include <BWAPI.h>
#include <MacroManager.h>

class TaskStreamObserver;
class TaskStream
{
  public:
    TaskStream(Task t0=Task(), Task t1=Task(), Task t2=Task(), Task t3=Task());
    ~TaskStream();
    void terminate();
    virtual void onRevoke(BWAPI::Unit* unit);
    void update();
    void clearPlanningData();
    bool updateStatus(); //returns true if planned additional resources (units, supply)

    void attach(TaskStreamObserver* obs, bool owned);
    void detach(TaskStreamObserver* obs);

    enum Status
    {
      None,
      Error_Task_Not_Specified,
      Error_Worker_Not_Specified,
      Error_Worker_Not_Owned,
      Error_Location_Not_Specified,
      Error_Location_Blocked,
      Error_Task_Requires_Addon,
      Waiting_For_Worker_To_Be_Ready,
      Waiting_For_Free_Time,
      Waiting_For_Earliest_Start_Time,
      Waiting_For_Required_Units,
      Waiting_For_Required_Tech,
      Waiting_For_Required_Upgrade,
      Waiting_For_Supply,
      Waiting_For_Gas,
      Waiting_For_Minerals,
      Executing_Task,
    };
    Status getStatus() const;
    std::string getStatusString() const;

    void setWorker(BWAPI::Unit* w);
    BWAPI::Unit* getWorker() const;

    void setBuildUnit(BWAPI::Unit* b);
    BWAPI::Unit* getBuildUnit() const;

    int getStartTime() const;
    int getFinishTime() const;
    int getFinishTime(BWAPI::UnitType t) const;

    void setTaskStarted(bool started);
    void completeTask();

    bool isWorkerReady() const;
    bool isLocationReady() const;

    void setTask(int index, Task t);
    Task& getTask(int index);

    void setUrgent(bool isUrgent);
    bool isUrgent() const;

    void printToScreen(int x, int y);
    TaskStream* forkCurrentTask();
  private:
    void notifyNewStatus();
    void notifyCompletedTask();
    void notifyForkedTask(TaskStream* newTS);
    void computeStatus();
    void computeBuildUnit();
    bool killSwitch;
    bool urgent;
    bool workerReady;
    bool locationReady;
    std::vector<Task> task;
    bool plannedAdditionalResources;
    BWAPI::Unit* worker;
    BWAPI::Unit* buildUnit;
    Status status;
    std::map<TaskStreamObserver*, bool > observers;
};
