#include <PrecompiledHeader.h>
#include <Macro/DeleteWorkBenchIfWorkerLost.h>
using namespace BWAPI;
DeleteWorkBenchIfWorkerLost::DeleteWorkBenchIfWorkerLost()
{
}
void DeleteWorkBenchIfWorkerLost::onFrame(TaskStream* ts)
{
  std::set<WorkBench*> killList;
  for each(WorkBench* wb in ts->workBenches)
  {
    if (wb->getWorker()==NULL || wb->getWorker()->exists()==false)
      killList.insert(wb);
  }
  for each(WorkBench* wb in killList)
    ts->workBenches.erase(wb);
}