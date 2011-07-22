#include <PrecompiledHeader.h>
#include <Macro/TerminateIfNoWorkBenches.h>
using namespace BWAPI;
TerminateIfNoWorkBenches::TerminateIfNoWorkBenches()
{
}
void TerminateIfNoWorkBenches::onFrame(TaskStream* ts)
{
  if (ts->workBenches.empty())
    ts->terminate();
}