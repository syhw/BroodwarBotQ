#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <UnitGroupManager.h>
#include "CSingleton.h"
class EnhancedUI : public CSingleton<EnhancedUI>
{
	friend class CSingleton<EnhancedUI>;
  public:
    void update() const;

  private:
    void drawBases() const;
    void drawTerrain() const;
    void drawProgress() const;
    void drawProgressBar(BWAPI::Position pos, double progressFaction, BWAPI::Color innerBar = BWAPI::Colors::Green) const;
};