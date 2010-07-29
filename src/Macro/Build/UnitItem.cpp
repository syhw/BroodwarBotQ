#include "UnitItem.h"
#include "BuildManager.h"
BuildManager*& UnitItem::getBuildManager()
{
  static BuildManager* buildManager;
  return buildManager;
}
UnitItem::UnitItem()
{
  this->type=BWAPI::UnitTypes::None;
  this->nonadditional=0;
  nonadditionalPosition=BWAPI::TilePositions::None;
}
UnitItem::UnitItem(BWAPI::UnitType type)
{
  this->type=type;
  this->nonadditional=0;
  nonadditionalPosition=BWAPI::TilePositions::None;
}

int UnitItem::getRemainingCount(int currentPlannedCount)
{
  //initialize counter
  int count=0;

  //if non-additional is not zero
  if (this->nonadditional!=0)
  {
    if (currentPlannedCount==-1)
      currentPlannedCount=UnitItem::getBuildManager()->getPlannedCount(this->type);

    count = this->nonadditional-currentPlannedCount;

    //make sure count is no less than 0
    if (count<0) count=0;
  }

  //loop through all additional unit items, and increment count as needed
  for(std::map<BWAPI::TilePosition, int>::iterator k=this->additional.begin();k!=this->additional.end();k++)
  {
    count+=k->second;
  }

  //return the total count
  return count;
}

void UnitItem::addAdditional(int count, BWAPI::TilePosition position)
{
  this->additional[position]+=count;
}

void UnitItem::setNonAdditional(int count, BWAPI::TilePosition position)
{
  this->nonadditional=count;
  this->nonadditionalPosition=position;
}

BWAPI::TilePosition UnitItem::decrementAdditional(BWAPI::TilePosition position)
{
  if (this->additional.empty()) return this->nonadditionalPosition;
  if (position==BWAPI::TilePositions::None)
  {
    (*this->additional.begin()).second--;
    position=(*this->additional.begin()).first;
    if ((*this->additional.begin()).second==0)
      this->additional.erase(this->additional.begin());
  }
  else
  {
    if (this->additional.find(position)==this->additional.end())
      return position;
    this->additional[position]--;
    if (this->additional[position]<=0)
      this->additional.erase(position);
  }
  return position;
}