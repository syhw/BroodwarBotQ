#pragma once
#include <list>
#include <set>
#include <map>
#include "Macro/Heap.h"
#include "Controller.h"
namespace Arbitrator
{
  template <class _Tp,class _Val>
  class Arbitrator
  {
  public:
    Arbitrator();
    bool setBid(Controller<_Tp,_Val>* c, _Tp obj, _Val bid);
    bool setBid(Controller<_Tp,_Val>* c, std::set<_Tp> objs, _Val bid);
    bool removeBid(Controller<_Tp,_Val>* c, _Tp obj);
    bool removeBid(Controller<_Tp,_Val>* c, std::set<_Tp> objs);
    bool removeAllBids(Controller<_Tp,_Val>* c);
    bool removeController(Controller<_Tp,_Val>* c);
    bool accept(Controller<_Tp,_Val>* c, _Tp obj, _Val bid);
    bool accept(Controller<_Tp,_Val>* c, std::set<_Tp> objs, _Val bid);
    bool accept(Controller<_Tp,_Val>* c, _Tp obj);
    bool accept(Controller<_Tp,_Val>* c, std::set<_Tp> objs);
    bool decline(Controller<_Tp,_Val>* c, _Tp obj, _Val bid);
    bool decline(Controller<_Tp,_Val>* c, std::set<_Tp> objs, _Val bid);
    bool hasBid(_Tp obj) const;
    const std::pair<Controller<_Tp,_Val>*, _Val>& getHighestBidder(_Tp obj) const;
    const std::list< std::pair<Controller<_Tp,_Val>*, _Val> > getAllBidders(_Tp obj) const;
    const std::set<_Tp>& getObjects(Controller<_Tp,_Val>* c) const;
    const std::set<_Tp>& getObjectsBidOn(Controller<_Tp,_Val>* c) const;
    void onRemoveObject(_Tp obj);
    _Val getBid(Controller<_Tp,_Val>* c, _Tp obj) const;
    void update();
  private:
    bool revokeOwnership(_Tp object, _Val bid, Controller<_Tp,_Val>* c = NULL);
    std::map<_Tp,Heap<Controller<_Tp,_Val>*, _Val> > bids;
    std::map<_Tp,Controller<_Tp,_Val>* > owner;
    std::map<Controller<_Tp,_Val>*, std::set<_Tp> > objectsOwned;
    std::map<Controller<_Tp,_Val>*, std::set<_Tp> > objectsBidOn;
    std::set<_Tp> updatedObjects;
    std::set<_Tp> objectsCanIncreaseBid;
    std::set<_Tp> unansweredObjects;
    std::set<_Tp> emptySet;
    Heap<Controller<_Tp,_Val>*, _Val> emptyHeap;
    std::pair<Controller<_Tp,_Val>*, _Val> defaultBidder;
    bool inUpdate;
    bool inOnOffer;
    bool inOnRevoke;
  };

  template <class _Tp,class _Val>
  Arbitrator<_Tp,_Val>::Arbitrator()
  {
    inUpdate=false;
    inOnOffer=false;
    inOnRevoke=false;
  }
  template <class _Tp,class _Val>
  //revokes the ownership of obj from its current owner unless the owner is c
  bool Arbitrator<_Tp,_Val>::revokeOwnership(_Tp obj, _Val bid, Controller<_Tp,_Val>* c)
  {
    std::map<_Tp,Controller<_Tp,_Val>* >::iterator ownerIter = owner.find(obj);
    if (ownerIter == owner.end())
    {
      return false;
    }
    Controller<_Tp,_Val>* theOwner = ownerIter->second;
    //remove ownership unless it is this controller
    if (theOwner == c)
    {
      return false;
    }
    inOnRevoke=true;
    theOwner->onRevoke(obj,bid);
    inOnRevoke=false;
    std::map<Controller<_Tp,_Val>*, std::set<_Tp> >::iterator objectsOwnedIter = objectsOwned.find(theOwner);
    if (objectsOwnedIter!=objectsOwned.end())
    {
      objectsOwnedIter->second.erase(obj);//remove this object from the set of objects owned by the former owner
      if (objectsOwnedIter->second.empty())
      {
        objectsOwned.erase(objectsOwnedIter);
      }
    }
    owner.erase(ownerIter);
    return true;
  }

  template <class _Tp,class _Val>
  bool Arbitrator<_Tp,_Val>::setBid(Controller<_Tp,_Val>* c, _Tp obj, _Val bid)
  {
    if (c == NULL || obj == NULL)
    {
      return false;
    }

    //can only increase bids of certain objects during update()
    std::map<_Tp,Heap<Controller<_Tp,_Val>*, _Val> >::iterator bidsIter = bids.find(obj);
    if (bidsIter != bids.end() && bidsIter->second.contains(c) && bid > bidsIter->second.get(c))
    {
      if (inOnRevoke || (inOnOffer && objectsCanIncreaseBid.find(obj) == objectsCanIncreaseBid.end()))
      {
        return false;
      }
    }

    //set the bid for this object and insert the object into the updated set
    bids[obj].set(c,bid);
    objectsBidOn[c].insert(obj);
    updatedObjects.insert(obj);
    return true;
  }

  template <class _Tp,class _Val>
  bool Arbitrator<_Tp,_Val>::setBid(Controller<_Tp,_Val>* c, std::set<_Tp> objs, _Val bid)
  {
    bool result = false;
    for (std::set<_Tp>::const_iterator o = objs.begin(); o != objs.end(); o++)
    {
      result |= setBid(c, *o, bid);
    }
    return result;
  }

  template <class _Tp,class _Val>
  bool Arbitrator<_Tp,_Val>::removeBid(Controller<_Tp,_Val>* c, _Tp obj)
  {
    if (c == NULL || obj == NULL)
    {
      return false;
    }
    std::map<_Tp,Heap<Controller<_Tp,_Val>*, _Val> >::iterator bidsIter = bids.find(obj);
    if (bidsIter != bids.end() && bidsIter->second.contains(c)) //check to see if the bid exists
    {
      bidsIter->second.erase(c); //if so, remove the bid
      if (bidsIter->second.empty())
      {
        bids.erase(bidsIter);
      }
      updatedObjects.insert(obj); //insert the object into the updated set
    }
    std::map<Controller<_Tp,_Val>*, std::set<_Tp> >::iterator objectsBidOnIter = objectsBidOn.find(c);
    if (objectsBidOnIter!=objectsBidOn.end())
    {
      objectsBidOnIter->second.erase(obj);
      if (objectsBidOnIter->second.empty())
      {
        objectsBidOn.erase(objectsBidOnIter);
      }
    }
    return true;
  }

  template <class _Tp,class _Val>
  bool Arbitrator<_Tp,_Val>::removeBid(Controller<_Tp,_Val>* c, std::set<_Tp> objs)
  {
    bool result = false;
    for (std::set<_Tp>::const_iterator o = objs.begin(); o != objs.end(); o++)
    {
      result |= removeBid(c, *o);
    }
    return result;
  }

  template <class _Tp,class _Val>
  bool Arbitrator<_Tp,_Val>::removeAllBids(Controller<_Tp,_Val>* c)
  {
    std::map<Controller<_Tp,_Val>*, std::set<_Tp> >::iterator objectsBidOnIter = objectsBidOn.find(c);
    if (objectsBidOnIter == objectsBidOn.end())
    {
      return false;
    }
    return removeBid(c,objectsBidOnIter->second);
  }

  template <class _Tp,class _Val>
  bool Arbitrator<_Tp,_Val>::removeController(Controller<_Tp,_Val>* c)
  {
    if (c == NULL) return false;
    //first - remove all bids
    removeAllBids(c);
    //second - remove all ownership
    std::map<Controller<_Tp,_Val>*, std::set<_Tp> >::iterator objectsOwnedIter = objectsOwned.find(c);
    if (objectsOwnedIter != objectsOwned.end())
    {
      for (std::set<_Tp>::const_iterator o = objectsOwnedIter->second.begin(); o != objectsOwnedIter->second.end(); o++)
      {
        std::map<_Tp,Controller<_Tp,_Val>* >::iterator ownerIter = owner.find(*o);
        if (ownerIter!=owner.end() && ownerIter->second == c)
        {
          owner.erase(ownerIter);
        }
      }
      objectsOwned.erase(objectsOwnedIter);
    }
    return true;
  }

  template <class _Tp,class _Val>
  bool Arbitrator<_Tp,_Val>::decline(Controller<_Tp,_Val>* c, _Tp obj, _Val bid)
  {
    if (c == NULL || obj == NULL)
    {
      return false;
    }
    if (!inOnOffer) //can only call decline from the onOffer() callback
    {
      return false;
    }
    if (hasBid(obj)==false || bids[obj].top().first != c) //only the top bidder/controller can decline an object
    {
      return false;
    }
    updatedObjects.insert(obj);
    unansweredObjects.erase(obj);
    objectsCanIncreaseBid.erase(obj);

    //must decrease bid via decline()
    std::map<_Tp,Heap<Controller<_Tp,_Val>*, _Val> >::iterator bidsIter = bids.find(obj);
    if (bidsIter!=bids.end() && bidsIter->second.contains(c) && bid>=bidsIter->second.get(c))
    {
      bid = 0;
    }
    if (bid <= 0) //could be set to 0 from 2 lines above or the user could have called with a bid <= 0
    {
      if (bidsIter!=bids.end() && bidsIter->second.contains(c))
      {
        bidsIter->second.erase(c);
        if (bidsIter->second.empty())
        {
          bids.erase(bidsIter);
        }
      }
      return true;
    }
    bids[obj].set(c, bid);
    return true;
  }

  template <class _Tp,class _Val>
  bool Arbitrator<_Tp,_Val>::decline(Controller<_Tp,_Val>* c, std::set<_Tp> objs, _Val bid)
  {
    bool result = false;
    for (std::set<_Tp>::const_iterator o = objs.begin(); o != objs.end(); o++)
    {
      result |= decline(c, *o, bid);
    }
    return result;
  }

  template <class _Tp,class _Val>
  bool Arbitrator<_Tp,_Val>::accept(Controller<_Tp,_Val>* c, _Tp obj)
  {
    if (c == NULL || obj == NULL)
    {
      return false;
    }
    if (!inOnOffer) //can only call accept from the onOffer() callback
    {
      return false;
    }
    if (hasBid(obj)==false || bids[obj].top().first != c) //only the top bidder/controller can accept an object
    {
      return false;
    }
    unansweredObjects.erase(obj);
    inOnOffer=false;
    revokeOwnership(obj,bids[obj].top().second,c);
    inOnOffer=true;
    owner[obj] = c; //set the new owner
    objectsOwned[c].insert(obj); //insert this object into the set of objects owned by this controller
    return true;
  }

  template <class _Tp,class _Val>
  bool Arbitrator<_Tp,_Val>::accept(Controller<_Tp,_Val>* c, std::set<_Tp> objs)
  {
    bool result = false;
    for (std::set<_Tp>::const_iterator o = objs.begin(); o != objs.end(); o++)
    {
      result |= accept(c, *o);
    }
    return result;
  }

  template <class _Tp,class _Val>
  bool Arbitrator<_Tp,_Val>::accept(Controller<_Tp,_Val>* c, _Tp obj, _Val bid)
  {
    //same idea as accept(Controller<_Tp,_Val>* c, _Tp obj), but the controller also specifies a new bid value
    if (c == NULL || obj == NULL)
    {
      return false;
    }
    if (!inOnOffer) //can only call accept from the onOffer() callback
    {
      return false;
    }
    if (hasBid(obj)==false || bids[obj].top().first != c) //only the top bidder/controller can accept an object
    {
      return false;
    }
    unansweredObjects.erase(obj);
    inOnOffer=false;
    revokeOwnership(obj,bids[obj].top().second,c);
    inOnOffer=true;
    owner[obj] = c; //set the new owner
    objectsOwned[c].insert(obj); //insert this object into the set of objects owned by this controller
    updatedObjects.insert(obj); //since the object was updated, insert it into the updated objects set

    //cannot decrease bid via accept()
    std::map<_Tp,Heap<Controller<_Tp,_Val>*, _Val> >::iterator bidsIter = bids.find(obj);
    if (bidsIter!=bids.end() && bidsIter->second.contains(c) && bid<bidsIter->second.get(c))
    {
      return true;
    }

    //update the bid for this object
    bids[obj].set(c,bid);
    return true;
  }

  template <class _Tp,class _Val>
  bool Arbitrator<_Tp,_Val>::accept(Controller<_Tp,_Val>* c, std::set<_Tp> objs, _Val bid)
  {
    bool result = false;
    for (std::set<_Tp>::const_iterator o = objs.begin(); o != objs.end(); o++)
    {
      result |= accept(c, *o, bid);
    }
    return result;
  }

  template <class _Tp,class _Val>
  bool Arbitrator<_Tp,_Val>::hasBid(_Tp obj) const
  {
    //returns true if the given object exists in the bids map
    return (bids.find(obj) != bids.end() && !bids.find(obj)->second.empty());
  }

  template <class _Tp,class _Val>
  const std::pair<Controller<_Tp,_Val>*, _Val>& Arbitrator<_Tp,_Val>::getHighestBidder(_Tp obj) const
  {
    //returns the controller at the top of the bid heap for this object
    if (bids.find(obj) == bids.end())
    {
      return defaultBidder;
    }
    return bids.find(obj)->second.top();
  }

  template <class _Tp,class _Val>
  const std::list< std::pair<Controller<_Tp,_Val>*, _Val> > Arbitrator<_Tp,_Val>::getAllBidders(_Tp obj) const
  {
    //returns all bidders for this object
    std::list< std::pair<Controller<_Tp,_Val>*, _Val> > bidders;
    if (bids.find(obj) == bids.end())
    {
      return bidders; //return empty list if we cannot find this object
    }

    Heap<Controller<_Tp,_Val>*, _Val> bid_heap = bids.find(obj)->second; //get the bid heap

    //push the bidders into the bidders list from top to bottom
    while(!bid_heap.empty())
    {
      bidders.push_back(bid_heap.top());
      bid_heap.pop();
    }
    return bidders;
  }

  template <class _Tp,class _Val>
  const std::set<_Tp>& Arbitrator<_Tp,_Val>::getObjects(Controller<_Tp,_Val>* c) const
  {
    //returns the set of objects owned by this bidder/controller
    if (objectsOwned.find(c) == objectsOwned.end())
    {
      return emptySet;
    }
    return objectsOwned.find(c)->second;
  }

  template <class _Tp,class _Val>
  const std::set<_Tp>& Arbitrator<_Tp,_Val>::getObjectsBidOn(Controller<_Tp,_Val>* c) const
  {
    //returns the set of objects bid on by this bidder/controller
    if (objectsBidOn.find(c) == objectsBidOn.end())
    {
      return emptySet;
    }
    return objectsBidOn.find(c)->second;
  }

  template <class _Tp,class _Val>
  void Arbitrator<_Tp,_Val>::onRemoveObject(_Tp obj)
  {
    //called from AIModule::onUnitDestroy, remove all memory of the object
    std::map<_Tp,Heap<Controller<_Tp,_Val>*, _Val> >::iterator bidsIter = bids.find(obj);
    if (bidsIter != bids.end())
    {
      Heap<Controller<_Tp,_Val>*, _Val> bid_heap=bidsIter->second; //get the bid heap

      //for each bidder, remove this object from the set of objects the bidder has bid on
      while(!bid_heap.empty())
      {
        Controller<_Tp,_Val>* bidder = bid_heap.top().first;
        bid_heap.pop();
        std::map<Controller<_Tp,_Val>*, std::set<_Tp> >::iterator objectsBidOnIter = objectsBidOn.find(bidder);
        if (objectsBidOnIter!=objectsBidOn.end())
        {
          objectsBidOnIter->second.erase(obj);
          if (objectsBidOnIter->second.empty())
          {
            objectsBidOn.erase(objectsBidOnIter);
          }
        }
      }
      bids.erase(bidsIter);
    }
    _Val temp = 0;
    revokeOwnership(obj,temp);

    std::set<_Tp>::iterator updatedObjectsIter = updatedObjects.find(obj);
    if (updatedObjectsIter != updatedObjects.end())
    {
      updatedObjects.erase(updatedObjectsIter);
    }
  }

  template <class _Tp,class _Val>
  _Val Arbitrator<_Tp,_Val>::getBid(Controller<_Tp,_Val>* c, _Tp obj) const
  {
    //returns the bid the given controller has on the given object

    std::map<_Tp,Heap<Controller<_Tp,_Val>*, _Val> >::iterator bidsIter = bids.find(obj);
    if (bidsIter!=bids.end() && bidsIter->second.contains(c))
    {
      return bidsIter->second.get(c);
    }
    //else
    return _Val(0);
  }

  template <class _Tp,class _Val>
  void Arbitrator<_Tp,_Val>::update()
  {
    this->inUpdate=true;
    bool first=true;
    //first we construct a map for the objects to offer
    std::map<Controller<_Tp,_Val>*, std::set<_Tp> > objectsToOffer;

    while(first || !objectsToOffer.empty())
    {
      first=false;
      objectsToOffer.clear();

      this->objectsCanIncreaseBid.clear();

      //go through all the updated objects
      for(std::set<_Tp>::iterator i = updatedObjects.begin(); i != updatedObjects.end(); ++i)
      {
        std::map<_Tp,Heap<Controller<_Tp,_Val>*, _Val> >::iterator bidsIter = bids.find(*i);
        std::map<_Tp,Controller<_Tp,_Val>* >::iterator ownerIter = owner.find(*i);
        if (bidsIter!=bids.end() && !bidsIter->second.empty()) //if there is a bid on this object
        {
          if (ownerIter == owner.end() || bidsIter->second.top().first != ownerIter->second) //if the top bidder is not the owner
            objectsToOffer[bidsIter->second.top().first].insert(*i); //make a note to offer it to the top bidder.
        }
        else
        {
          //no bids on this object, remove it from the owner if there is one
          if (ownerIter != owner.end())
          {
            revokeOwnership(*i,0);
          }
        }
      }
      //reset updated objects
      updatedObjects.clear();

      //offer the objects to the top bidders
      for(std::map< Controller<_Tp,_Val>*, std::set<_Tp> >::iterator i = objectsToOffer.begin(); i != objectsToOffer.end(); ++i)
      {
        objectsCanIncreaseBid=i->second;
        unansweredObjects=i->second;

        inOnOffer=true;
        i->first->onOffer(i->second);
        inOnOffer=false;

        //decline all unanswered objects
        for(std::set<_Tp>::iterator j=unansweredObjects.begin();j!=unansweredObjects.end();++j)
        {
          decline(i->first,*j,0);
        }
      }
    }
    this->inUpdate=false;
  }
}