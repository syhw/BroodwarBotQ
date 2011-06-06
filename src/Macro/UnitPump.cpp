#include <PrecompiledHeader.h>
#include "Macro/UnitPump.h"
using namespace BWAPI;
UnitPump::UnitPump(UnitType t)
{
  type = t;
}
void UnitPump::update(TaskStream* ts)
{
  for (int i=0;i<4;i++)
    if (ts->getTask(i)==Task())
      ts->setTask(i,Task(type));
}