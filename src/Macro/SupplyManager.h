#pragma once
#include "Macro/Arbitrator.h"
#include <BWAPI.h>
class SupplyManager
{
  public:
    static SupplyManager* create();
    static void destroy();
    void update();
    int lastFrameCheck;
    int initialSupplyTotal;
    int initialSupplyProviderCount;
  private:
    SupplyManager();
    ~SupplyManager();
};
extern SupplyManager* TheSupplyManager;