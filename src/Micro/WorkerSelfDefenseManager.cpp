#include <PrecompiledHeader.h>
#include <WorkerSelfDefenseManager.h>
using namespace BWAPI;
using namespace std;
WorkerSelfDefenseManager::WorkerSelfDefenseManager(Arbitrator::Arbitrator<BWAPI::Unit*,double>* arbitrator)
{
  this->arbitrator = arbitrator;
}
void WorkerSelfDefenseManager::onOffer(std::set<BWAPI::Unit*> units)
{
  for(set<Unit*>::iterator u = units.begin(); u != units.end(); u++)
  {
    if ((*u)->getType().isWorker() && !getAttackers[*u].empty())
    {
      arbitrator->accept(this,*u);
      myWorkers.insert(*u);
    }
    else
    {
      arbitrator->decline(this,*u,0);
    }
  }
}
void WorkerSelfDefenseManager::onRevoke(BWAPI::Unit* unit, double bid)
{
  onRemoveUnit(unit);
}
void WorkerSelfDefenseManager::update()
{
  UnitGroup attackingEnemyUnits = SelectAllEnemy()(isCompleted)(isStartingAttack);
  UnitGroup workers = SelectAll()(isCompleted)(isWorker);

  int frame = Broodwar->getFrameCount();
  int framem24=frame-24;

  //update getAttackers to set last attack frames
  for each(Unit* u in attackingEnemyUnits)
  {
    Unit* target = u->getTarget();
    if (target == NULL) target = u->getOrderTarget();
    if (target != NULL)
    {
      UnitGroup nearby = workers.inRadius(40,u->getPosition())+workers.inRadius(40,target->getPosition());
      for each(Unit* n in nearby)
      {
        if (getAttackers[n].contains(u)==false || getAttackers[n].get(u)<framem24)
        {
          getAttackers[n].set(u,framem24);
        }
      }
      getAttackers[target].set(u,frame);
    }
  }

  //remove units from getAttackers that have not attacked in a while
  for(std::map<Unit*, Arbitrator::Heap<Unit*,int> >::iterator i =getAttackers.begin(); i!=getAttackers.end(); i++)
  {
    while(i->second.empty()==false && (i->second.top().first->exists()==false || i->second.top().second+48<frame))
    {
      i->second.pop();
    }
  }

  //bid on workers that are being attacked
  for each(Unit* u in workers)
  {
    if (getAttackers[u].empty()==false)
      arbitrator->setBid(this, u, 50);
  }

  //control my workers to attack their attackers
  for each(Unit* u in myWorkers)
  {
    Broodwar->drawTextMap(u->getPosition().x(),u->getPosition().y(),"%d",getAttackers[u].size());
    if (getAttackers[u].empty())
    {
      arbitrator->removeBid(this,u);
    }
    else
    {
      if (u->getOrder()!=Orders::AttackUnit)
      {
        u->attack(getAttackers[u].top().first);
      }
    }
  }
}
std::string WorkerSelfDefenseManager::getName() const
{
  return "Worker Self Defense Manager";
}
std::string WorkerSelfDefenseManager::getShortName() const
{
  return "Worker Def";
}
void WorkerSelfDefenseManager::onRemoveUnit(BWAPI::Unit* unit)
{
  myWorkers.erase(unit);
}