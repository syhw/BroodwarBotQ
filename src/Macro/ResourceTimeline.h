#pragma once
#include "Resources.h"
#include <map>

//Timeline of resources. Enables queries on when resources will be available
class ResourceTimeline
{
  public:
    ResourceTimeline();
    void reset(const Resources &r, int supplyTotal, double mgr, double ggr);
    Resources getActualResourcesAtTime(int frame);
    Resources getAvailableResourcesAtTime(int frame);
    int getSupplyTotalAtTime(int frame);
    int getFinalSupplyAvailable();
    int getFinalSupplyTotal();
    bool reserveResources(int frame, const Resources &r);
    int getFirstValidTime(const Resources &r);
    int getFirstTimeWhenSupplyIsNoGreaterThan(int supplyAmount);
    enum ErrorCode
    {
      None,
      Insufficient_Minerals,
      Insufficient_Gas,
      Insufficient_Supply
    };
    ErrorCode getLastError() const;
    bool registerSupplyIncrease(int frame, int supply);
    bool unregisterSupplyIncrease(int frame, int supply);
//  private:
    ErrorCode lastError;
    double mineralGatherRate;
    double gasGatherRate;
    Resources currentResources;
    int currentSupplyTotal;
    std::map<int, Resources > resourceEvents;
    std::map<int, int > supplyIncreaseEvents;
};