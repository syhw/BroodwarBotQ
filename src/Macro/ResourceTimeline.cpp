#include <PrecompiledHeader.h>
#include "ResourceTimeline.h"
using namespace std;
using namespace BWAPI;
ResourceTimeline::ResourceTimeline()
{
  currentResources = Resources();
  resourceEvents.clear();
  supplyIncreaseEvents.clear();
  mineralGatherRate = 0;
  gasGatherRate = 0;
  lastError = None;
}
void ResourceTimeline::reset(const Resources &r, int supplyTotal, double mgr, double ggr)
{
  currentResources = r;
  currentSupplyTotal = supplyTotal;
  resourceEvents.clear();
  supplyIncreaseEvents.clear();
  mineralGatherRate = mgr;
  gasGatherRate = ggr;
  lastError = None;
}

Resources ResourceTimeline::getActualResourcesAtTime(int frame)
{
  if (resourceEvents.find(frame)==resourceEvents.end())
    resourceEvents.insert(std::make_pair(frame,Resources()));
  int lastFrame = Broodwar->getFrameCount();
  Resources res = currentResources;
  for(std::map<int, Resources>::iterator i=resourceEvents.begin();i!=resourceEvents.end();i++)
  {
    int currentFrame = i->first;
    int duration = currentFrame - lastFrame;
    res.addMinerals((int)(duration*mineralGatherRate));
    res.addGas((int)(duration*gasGatherRate));
    Resources resi=i->second;
    if (resi.getMinerals()<0) resi.setMinerals(0);
    if (resi.getGas()<0) resi.setGas(0);
    if (resi.getSupply()<0) resi.setSupply(0);
    res+=resi;
    if (currentFrame == frame)
      break;
    lastFrame = currentFrame;
  }
  return res;
}
Resources ResourceTimeline::getAvailableResourcesAtTime(int frame)
{
  if (resourceEvents.find(frame)==resourceEvents.end())
    resourceEvents.insert(std::make_pair(frame,Resources()));
  int lastFrame = Broodwar->getFrameCount();
  Resources res = currentResources;
  Resources availRes;
  for(std::map<int, Resources>::iterator i=resourceEvents.begin();i!=resourceEvents.end();i++)
  {
    int currentFrame = i->first;
    int duration = currentFrame - lastFrame;
    res.addMinerals((int)(duration*mineralGatherRate));
    res.addGas((int)(duration*gasGatherRate));
    res+=i->second;
    if (currentFrame == frame)
    {
      availRes=res;
    }
    else if (currentFrame>frame)
    {
      if (res.getMinerals()<availRes.getMinerals())
        availRes.setMinerals(res.getMinerals());
      if (res.getGas()<availRes.getGas())
        availRes.setGas(res.getGas());
      if (res.getSupply()<availRes.getSupply())
        availRes.setSupply(res.getSupply());
    }
    lastFrame = currentFrame;
  }
  return availRes;
}
int ResourceTimeline::getSupplyTotalAtTime(int frame)
{
  int supplyTotal = currentSupplyTotal;
  for(std::map<int, int>::iterator i=supplyIncreaseEvents.begin();i!=supplyIncreaseEvents.end();i++)
  {
    if (i->first > frame) break;
    supplyTotal+=i->second;
  }
  return supplyTotal;
}
int ResourceTimeline::getFinalSupplyAvailable()
{
  Resources res = currentResources;
  int supply = (int)(currentResources.getSupply());
  for(std::map<int, Resources>::iterator i=resourceEvents.begin();i!=resourceEvents.end();i++)
  {
    supply+=(int)(i->second.getSupply());
  }
  return supply;
}
int ResourceTimeline::getFinalSupplyTotal()
{
  int supplyTotal = currentSupplyTotal;
  for(std::map<int, int>::iterator i=supplyIncreaseEvents.begin();i!=supplyIncreaseEvents.end();i++)
  {
    supplyTotal+=i->second;
  }
  return supplyTotal;
}

bool ResourceTimeline::reserveResources(int frame, const Resources &r)
{
  //reseves the specified amount of resources at the specified frame, or returns false if predicted resource amounts are insufficient

  //reserve specified amount of resources at the specified frame
  resourceEvents[frame]-=r;

  //rest of the function checks to see if the resource time line is still valid
  int lastFrame = Broodwar->getFrameCount();

  //maintain running count of resources over time
  Resources res = currentResources;

  bool isValid = true;

  //iterate over resource events
  for(std::map<int, Resources>::iterator i=resourceEvents.begin();i!=resourceEvents.end();i++)
  {
    //get frame for this event and duration since last event
    int currentFrame = i->first;
    int duration = currentFrame - lastFrame;

    //add minerals and gas based on gather rates
    res.addMinerals(duration*mineralGatherRate);
    res.addGas(duration*gasGatherRate);

    //add resources in this event
    res+=i->second;

    //check to see if resources are still valid
    if (res.isValid()==false)
    {
      isValid = false;
      break;
    }

    //update last frame for duration calculation
    lastFrame = currentFrame;
  }

  //if resource time line is valid, we have no error, else we determine the error
  if (isValid)
    lastError = None;
  else
  {
    //determine why resources are invalid and set error code accordindly
    if (res.getSupply()<0)
      lastError = Insufficient_Supply;
    else if (res.getGas()<0)
      lastError = Insufficient_Gas;
    else
      lastError = Insufficient_Minerals;

    //unreserve resources so time line becomes valid again
    resourceEvents[frame]+=r;
  }
  return isValid;
}
int ResourceTimeline::getFirstValidTime(const Resources &r)
{
  // determines the first frame when we can reserve the specified amount of
  // resources without making the time line invalid, or -1 if never
  int frame = Broodwar->getFrameCount();
 
  //reserve resources for current frame and then find the last invalid frame
  resourceEvents[frame]-=r;
  int lastFrame = Broodwar->getFrameCount();
  int lastInvalidEventFrame = -1;

  //we our resources become invalid, they will be saved here
  Resources invalidRes;

  //maintain running count of resources over time
  Resources res = currentResources;

  bool isValid = false;

  //holds first valid frame after the last invalid frame
  int validEventFrame = -1;

  //iterate over resource events
  for(std::map<int, Resources>::iterator i=resourceEvents.begin();i!=resourceEvents.end();i++)
  {
    //get frame for this event and duration since last event
    int currentFrame = i->first;
    int duration = currentFrame - lastFrame;

    //add minerals and gas based on gather rates
    res.addMinerals(duration*mineralGatherRate);
    res.addGas(duration*gasGatherRate);

    //add resources in this event
    res+=i->second;

    //see if resources are valid
    if (res.isValid()==false)
    {
      //if not, update last invalid frame
      invalidRes=res;
      lastInvalidEventFrame = currentFrame;
      isValid = false;
    }
    else
    {
      //if so, and it wasn't valid before, update valid event frame
      if (!isValid)
        validEventFrame = currentFrame;
      isValid = true;
    }

    //update last frame for duration calculation
    lastFrame = currentFrame;
  }
  //unreserve resources now that our calculations are done
  resourceEvents[frame]+=r;

  if (lastInvalidEventFrame==-1)
    return frame; //if resources never become invalid, return current frame
  double t=lastInvalidEventFrame;

  //if we have invalid resoures with no hope of them becoming valid
  if (invalidRes.getSupply()<-0.0001 || 
     (invalidRes.getGas()<-0.0001 && fabs(gasGatherRate)<0.0001) ||
     (invalidRes.getMinerals()<-0.0001 && fabs(mineralGatherRate)<0.0001))
  {
    //return valid event frame if resources eventually became valid
    if (validEventFrame>lastInvalidEventFrame)
      return validEventFrame;

    //otherwise, resources never became valid, so determine error and return never
    if (invalidRes.getSupply()<-0.0001)
      lastError = Insufficient_Supply;
    else if (invalidRes.getGas()<-0.0001 && fabs(gasGatherRate)<0.0001)
      lastError = Insufficient_Gas;
    else if (invalidRes.getMinerals()<-0.0001 && fabs(mineralGatherRate)<0.0001)
      lastError = Insufficient_Minerals;
    return -1; //return never
  }

  //if our minerals are invalid, compute when they will become valid based on mineral gather rate
  if (invalidRes.getMinerals()<-0.001)
  {
    double t2=lastInvalidEventFrame-(invalidRes.getMinerals()-1)/mineralGatherRate;
    if (t2>t)
    {
      t=t2;
      lastError = Insufficient_Minerals;
    }
  }
  //if our gass is invalid, compute when it will become valid based on gas gather rate
  if (invalidRes.getGas()<-0.001)
  {
    double t2=lastInvalidEventFrame-(invalidRes.getGas()-1)/gasGatherRate;
    if (t2>t)
    {
      t=t2;
      lastError = Insufficient_Gas;
    }
  }
  //compute ceiling of double value
  int ti=(int)t;
  if (t>ti) ti++;
  return ti;
}
int ResourceTimeline::getFirstTimeWhenSupplyIsNoGreaterThan(int supplyAmount)
{
  //returns the first frame when the supply available is not greater than the specified amount
  int frame = Broodwar->getFrameCount();
  Resources res = currentResources;
  int supply = (int)(currentResources.getSupply());
  if (supply<=supplyAmount) return frame;
  for(std::map<int, Resources>::iterator i=resourceEvents.begin();i!=resourceEvents.end();i++)
  {
    frame = i->first;
    supply+=(int)(i->second.getSupply());
    if (supply<=supplyAmount) return frame;
  }
  return -1; //return never if the condition never occurs
}
ResourceTimeline::ErrorCode ResourceTimeline::getLastError() const
{
  return lastError;
}
bool ResourceTimeline::registerSupplyIncrease(int frame, int supply)
{
  if (supply<0) return false;
  int currentTotal=getSupplyTotalAtTime(frame);
  int newTotal = currentTotal + supply;
  //take into account the 200*2 supply cap
  if (newTotal > 400) newTotal = 400;
  supply = newTotal-currentTotal;  
  resourceEvents[frame].addSupply(supply);
  supplyIncreaseEvents[frame]+=supply;
  return true;
}
bool ResourceTimeline::unregisterSupplyIncrease(int frame, int supply)
{
  if (supply<0) return false;
  if (supplyIncreaseEvents[frame]<supply)
    return false;
  resourceEvents[frame].addSupply(-supply);
  supplyIncreaseEvents[frame]-=supply;
  return true;
}