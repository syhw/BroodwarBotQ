#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <set>
#include <MacroManager.h>
class MacroBase : public TaskStreamObserver
{
  public:
  static MacroBase* CreateBaseNow(BWTA::BaseLocation* b, bool getGas = true);
  static MacroBase* CreateBaseWhenPossible(BWTA::BaseLocation* b, bool getGas = true);
  static MacroBase* CreateBaseAtFrame(BWTA::BaseLocation* b, int frame, bool getGas = true);
  static MacroBase* RecognizeExistingBase(BWTA::BaseLocation* b, BWAPI::Unit* depot);
  TaskStream* rebuildBase();
  TaskStream* rebuildRefinery();
  BWTA::BaseLocation* getBaseLocation() const;
  BWAPI::Unit* getResourceDepot() const;
  BWAPI::Unit* getRefinery() const;
  const std::set<BWAPI::Unit*>& getMinerals() const;
  const std::set<BWAPI::Unit*>& getGeysers() const;
  TaskStream* getTaskStream() const;
  TaskStream* getRefineryTaskStream() const;
  void completedTask(TaskStream* ts, const Task &t);
  void setPaused(bool paused);
  bool isPaused() const;
  bool isReady() const;
  bool isActive() const;
  void update();
  void onUnitDestroy(BWAPI::Unit* u);

private:
  MacroBase(BWTA::BaseLocation* b);
  BWTA::BaseLocation* baseLocation;
  BWAPI::Unit* resourceDepot;
  BWAPI::Unit* refinery;
  bool ready;
  bool paused;
  TaskStream* depot_ts;
  TaskStream* refinery_ts;
};