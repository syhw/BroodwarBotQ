#pragma once
#include "Macro/MacroManager/TaskStreamObserver.h"
#include "Macro/MacroManager/TaskStream.h"
class BasicWorkerFinder : public TaskStreamObserver
{
  public:
    virtual void update(TaskStream* ts);
    virtual void attached(TaskStream* ts);
    virtual void newStatus(TaskStream* ts);
    virtual std::string getName() const;
    virtual std::string getShortName() const;
  private:
    TaskStream* ts;
};
