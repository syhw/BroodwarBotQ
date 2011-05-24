#include <PrecompiledHeader.h>
#include <TerminateIfEmpty.h>
using namespace BWAPI;
TerminateIfEmpty::TerminateIfEmpty()
{
}
void TerminateIfEmpty::newStatus(TaskStream* ts)
{
  if (ts->getStatus() == TaskStream::Error_Task_Not_Specified)
    ts->terminate();
}