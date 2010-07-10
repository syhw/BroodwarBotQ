#pragma once
#include <CSingleton.h>
#include <Arbitrator.h>
#include <BWAPI.h>
#include <BaseManager.h>
#include "BaseObject.h"

class WorkerManager : public Arbitrator::Controller<BWAPI::Unit*,double>, public CSingleton<WorkerManager>, public BaseObject
{
	friend class CSingleton<WorkerManager>;

	private:
		WorkerManager();
		~WorkerManager();

  public:
    class WorkerData
    {
      public:
        WorkerData() {resource = NULL; lastFrameSpam = 0;}
        BWAPI::Unit* resource;
        BWAPI::Unit* newResource;
        int lastFrameSpam;
    };
    void setBaseManager(BaseManager* baseManager);
    virtual void onOffer(std::set<BWAPI::Unit*> units);
    virtual void onRevoke(BWAPI::Unit* unit, double bid);
    virtual void update();
    virtual std::string getName() const;

#ifdef BW_QT_DEBUG
    // Qt interface
    virtual QWidget* createWidget(QWidget* parent) const;
    virtual void refreshWidget(QWidget* widget) const;
#endif

    void onUnitDestroy(BWAPI::Unit* unit);
    void setWorkersPerGas(int count);
    Arbitrator::Arbitrator<BWAPI::Unit*,double>* arbitrator;
    BaseManager* baseManager;
    std::map<BWAPI::Unit*,WorkerData> workers; // <worker, workerData>
    std::map<BWAPI::Unit*, std::set<BWAPI::Unit*> > currentWorkers; // <mineral block, worker>
    std::map<BWAPI::Unit*, Base*> resourceBase; // <min/gas, base>
    std::map<BWAPI::Unit*, int> desiredWorkerCount; // <min/gas, nbWorkers>
    std::vector<std::pair<BWAPI::Unit*, int> > mineralOrder; // <min block, interest> depending on ressource quantity - 2* distance to the base.
    int mineralOrderIndex;
    int lastSCVBalance;
    std::set<Base*> basesCache;
    int WorkersPerGas;
    
  private:
    void rebalanceWorkers();
    void updateWorkerAssignments();
};