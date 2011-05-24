#pragma once
#include <map>
#include <set>
#include <BWAPI.h>
#include <Arbitrator.h>
#include <BWTA.h>
#include "MacroBase.h"
class BorderManager;
class MacroManager;
class MacroBase;
class MacroBaseManager
{
  public:
    static MacroBaseManager* create();
    static void destroy();
    void setBorderManager(BorderManager* borderManager);
    void update();
    MacroBase* getBase(BWTA::BaseLocation* location);
    MacroBase* expandNow(BWTA::BaseLocation* location = NULL, bool getGas = true);
    MacroBase* expandWhenPossible(BWTA::BaseLocation* location = NULL, bool getGas = true);
    MacroBase* expandAtFrame(int frame, BWTA::BaseLocation* location = NULL, bool getGas = true);

    const std::set<MacroBase*>& getActiveBases() const;
    const std::set<MacroBase*>& getReadyBases() const;
    const std::set<MacroBase*>& getAllBases() const;
    const std::set<MacroBase*>& getDestroyedBases() const;

    std::string getName();
    void onUnitDestroy(BWAPI::Unit* unit);

  private:
    MacroBaseManager();
    ~MacroBaseManager();
    BorderManager* borderManager;
    std::map<BWTA::BaseLocation*,MacroBase*> location2base;
    std::set<MacroBase*> activeBases;
    std::set<MacroBase*> readyBases;
    std::set<MacroBase*> allBases;
    std::set<MacroBase*> destroyedBases;
};
extern MacroBaseManager* TheMacroBaseManager;