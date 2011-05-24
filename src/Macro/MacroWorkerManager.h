#pragma once
#include <Arbitrator.h>
#include <BWAPI.h>
#include <MacroBaseManager.h>

class MacroManager;
class MacroWorkerManager : public Arbitrator::Controller<BWAPI::Unit*,double>
{
  public:
    class WorkerData
    {
      public:
        WorkerData() {resource = NULL; lastFrameSpam = 0;}
        BWAPI::Unit* resource;
        BWAPI::Unit* newResource;
        int lastFrameSpam;
    };
    static MacroWorkerManager* create();
    static void destroy();
    virtual void onOffer(std::set<BWAPI::Unit*> units);
    virtual void onRevoke(BWAPI::Unit* unit, double bid);
    virtual void update();
    virtual std::string getName() const;
    virtual std::string getShortName() const;

    void onRemoveUnit(BWAPI::Unit* unit);
    void setWorkersPerGas(int count);
    double getMineralRate() const;
    double getGasRate() const;
    int getOptimalWorkerCount() const;
    void enableAutoBuild();
    void disableAutoBuild();
    void setAutoBuildPriority(int priority);
  private:
    MacroWorkerManager();
    ~MacroWorkerManager();
    std::map<BWAPI::Unit*,WorkerData> workers;
    std::map<BWAPI::Unit*, std::set<BWAPI::Unit*> > currentWorkers;
    std::map<BWAPI::Unit*, MacroBase*> resourceBase;
    std::map<BWAPI::Unit*, int> desiredWorkerCount;
    std::vector<std::pair<BWAPI::Unit*, int> > mineralOrder;
    int mineralOrderIndex;
    int lastSCVBalance;
    std::set<MacroBase*> basesCache;
    int WorkersPerGas;
    
    void rebalanceWorkers();
    void updateWorkerAssignments();
    double mineralRate;
    double gasRate;
    bool autoBuild;
    int autoBuildPriority;
    int optimalWorkerCount;
};
extern MacroWorkerManager* TheMacroWorkerManager;