#pragma once
#include <MacroManager/TaskStreamObserver.h>
#include <MacroManager/TaskStream.h>
class BasicTaskExecutor : public TaskStreamObserver
{
  public:
    static BasicTaskExecutor* getInstance();
    virtual void attached(TaskStream* ts);
    virtual void detached(TaskStream* ts);
    virtual void newStatus(TaskStream* ts);
    virtual void completedTask(TaskStream* ts, const Task &t);
    virtual void update(TaskStream* ts);
    void computeIsExecuting(TaskStream* ts);
    void computeIsCompleted(TaskStream* ts);
    void computeIsReady(TaskStream* ts);
    void getReady(TaskStream* ts);
    void execute(TaskStream* ts);
  private:
    struct data
    {
      bool isReady;
      int targetLevel;
    };
    std::map< TaskStream*, data > taskStreams;
};
