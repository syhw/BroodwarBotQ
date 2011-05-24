#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <UnitGroupManager.h>
class EnhancedUI
{
  public:
    void update() const;

  private:
    void drawBases() const;
    void drawTerrain() const;
    void drawProgress() const;
    void drawProgressBar(BWAPI::Position pos, double progressFaction, BWAPI::Color innerBar = BWAPI::Colors::Green) const;
};