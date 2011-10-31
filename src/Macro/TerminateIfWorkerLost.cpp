#include <PrecompiledHeader.h>
#include "Macro/TerminateIfWorkerLost.h"
using namespace BWAPI;
TerminateIfWorkerLost::TerminateIfWorkerLost()
{
}
void TerminateIfWorkerLost::newStatus(TaskStream* ts)
{
  if (ts->getStatus() == TaskStream::Error_Worker_Not_Specified || ts->getStatus() == TaskStream::Error_Worker_Not_Owned)
    ts->terminate();
}