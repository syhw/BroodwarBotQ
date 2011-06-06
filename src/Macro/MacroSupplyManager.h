#pragma once
#include "Macro/Arbitrator.h"
#include <BWAPI.h>
#include "Macro/MacroManager.h"
class MacroSupplyManager
{
  public:
    static MacroSupplyManager* create();
    static void destroy();
    void update();
    int lastFrameCheck;
    int initialSupplyTotal;
    int initialSupplyProviderCount;
  private:
    MacroSupplyManager();
    ~MacroSupplyManager();
};
extern MacroSupplyManager* TheMacroSupplyManager;