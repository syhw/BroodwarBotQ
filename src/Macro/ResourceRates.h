#pragma once
#include "Resources.h"
#include <map>
class ResourceRates
{
  public:
    static ResourceRates* create();
    static void destroy();
    Resources getGatherRate() const;
    void update();
  private:
    ResourceRates();
    ~ResourceRates();
    Resources gatherRate;
    int gatherRateStepSize;
    std::list<Resources> resourceHistory;
};
extern ResourceRates* TheResourceRates;