#pragma once
#include "Macro/Arbitrator.h"
#include <BWAPI.h>
#include "Macro/MacroManager.h"
class MacroDependencyResolver
{
  public:
    static MacroDependencyResolver* create();
    static void destroy();
    void update();
    int lastFrameCheck;
  private:
    MacroDependencyResolver();
    ~MacroDependencyResolver();
};
extern MacroDependencyResolver* TheMacroDependencyResolver;