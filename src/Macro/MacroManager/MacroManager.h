#pragma once
#include <BWAPI.h>
#include <BWSAL.h>
#include <Arbitrator.h>
#include <ResourceTimeline.h>
#include <UnitCountTimeline.h>
#include <TechTimeline.h>
#include <UpgradeTimeline.h>
#include <MacroManager/WorkerTaskTimeline.h>
#include <MacroManager/LarvaTimeline.h>
#include <Task.h>
class TaskStream;
class MacroManager : public Arbitrator::Controller<BWAPI::Unit*,double>
{
  public:
    static MacroManager* create();
    static void destroy();
    MacroManager();
    ~MacroManager();
    virtual void onOffer(std::set<BWAPI::Unit*> units);
    virtual void onRevoke(BWAPI::Unit* unit, double bid);
    virtual void update();
    std::string getName() const;
    std::string getShortName() const;

    /** Inserts the given task stream right above the given one */
    bool insertTaskStreamAbove(TaskStream* newTS, TaskStream* existingTS);

    /** Inserts the given task stream right below the given one */
    bool insertTaskStreamBelow(TaskStream* newTS, TaskStream* existingTS);

    /** Swaps the position of the two given task streams */
    bool swapTaskStreams(TaskStream* a, TaskStream* b);

    /** Gets the task streams of which this unit is a worker */
    const std::set<TaskStream*>& getTaskStreams(BWAPI::Unit* unit) const;

    std::list< TaskStream* > taskStreams;
    std::set< TaskStream* > killSet;
    ResourceTimeline rtl;
    UnitCountTimeline uctl;
    TechTimeline ttl;
    UpgradeTimeline utl;
    Resources spentResources;
    WorkerTaskTimeline wttl;
    LarvaTimeline ltl;
    std::map< BWAPI::Unit*, std::set<TaskStream*> > unitToTaskStreams;
    std::set< BWAPI::Unit*> ownedUnits;
    std::map< int, std::list< std::pair<TaskStream*, Task > > > plan;
    bool taskstream_list_visible;
};
extern MacroManager* TheMacroManager;