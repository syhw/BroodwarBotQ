#pragma once
#include "Macro/Arbitrator.h"
#include <BWAPI.h>
#include "Macro/MacroManager.h"
class MacroBuildOrderManager
{
  public:
    static MacroBuildOrderManager* create();
    static void destroy();
    void build(int count, BWAPI::UnitType t, int priority, BWAPI::TilePosition seedPosition=BWAPI::TilePositions::None);
    void buildAdditional(int count, BWAPI::UnitType t, int priority, BWAPI::TilePosition seedPosition=BWAPI::TilePositions::None);
    void research(BWAPI::TechType t, int priority);
    void upgrade(int level, BWAPI::UpgradeType t, int priority);
  private:
    MacroBuildOrderManager();
    ~MacroBuildOrderManager();
};
extern MacroBuildOrderManager* TheMacroBuildOrderManager;