#include <TerminateIfFinished.h>
using namespace BWAPI;
TerminateIfFinished::TerminateIfFinished()
{
}
void TerminateIfFinished::onCompletedTask(TaskStream* ts)
{
  if (ts->getStatus() == TaskStream::Task_Stream_Finished)
    ts->terminate();
}