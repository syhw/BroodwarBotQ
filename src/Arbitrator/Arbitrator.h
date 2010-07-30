#pragma once
#include <list>
#include <set>
#include <map>
#include "Heap.h"
#include "Controller.h"
#include "CSingleton.h"
namespace Arbitrator
{
  template <class _Tp,class _Val>
  class Arbitrator : public CSingleton<Arbitrator<_Tp, _Val> >
  {
	  friend class CSingleton<Arbitrator<_Tp, _Val> >;

  public:
   
    inline bool setBid(Controller<_Tp,_Val>* c, _Tp obj, _Val bid);
    inline bool setBid(Controller<_Tp,_Val>* c, std::set<_Tp> objs, _Val bid);
    inline bool removeBid(Controller<_Tp,_Val>* c, _Tp obj);
    inline bool removeBid(Controller<_Tp,_Val>* c, std::set<_Tp> objs);
    inline bool accept(Controller<_Tp,_Val>* c, _Tp obj, _Val bid);
   inline  bool accept(Controller<_Tp,_Val>* c, std::set<_Tp> objs, _Val bid);
   inline  bool accept(Controller<_Tp,_Val>* c, _Tp obj);
   inline  bool accept(Controller<_Tp,_Val>* c, std::set<_Tp> objs);
    inline bool decline(Controller<_Tp,_Val>* c, _Tp obj, _Val bid);
    inline bool decline(Controller<_Tp,_Val>* c, std::set<_Tp> objs, _Val bid);
    inline bool hasBid(_Tp obj) const;
    inline const std::pair<Controller<_Tp,_Val>*, _Val>& getHighestBidder(_Tp obj) const;
    inline const std::list< std::pair<Controller<_Tp,_Val>*, _Val> > getAllBidders(_Tp obj) const;
    inline const std::set<_Tp>& getObjects(Controller<_Tp,_Val>* c) const;
    inline void onRemoveObject(_Tp obj);
    inline _Val getBid(Controller<_Tp,_Val>* c, _Tp obj) const;
    inline void update();
  private:
	Arbitrator();
    std::map<_Tp,Heap<Controller<_Tp,_Val>*, _Val> > bids;
    std::map<_Tp,Controller<_Tp,_Val>* > owner;
    std::map<Controller<_Tp,_Val>*, std::set<_Tp> > objects;
    std::set<_Tp> updatedObjects;
    std::set<_Tp> objectsCanIncreaseBid;
    std::set<_Tp> unansweredObjected;
    bool inUpdate;
    bool inOnOffer;
    bool inOnRevoke;
  };
}

#include "Arbitrator.inl"