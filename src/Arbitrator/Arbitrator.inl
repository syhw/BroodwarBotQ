#pragma once

namespace Arbitrator
{

	template <class _Tp,class _Val>
	bool Arbitrator<_Tp,_Val>::setBid(Controller<_Tp,_Val>* c, _Tp obj, _Val bid)
	{
		if (c == NULL || obj == NULL)
			return false;
		bids[obj].set(c,bid);
		updatedObjects.insert(obj);
		return true;
	}

	template <class _Tp,class _Val>
	bool Arbitrator<_Tp,_Val>::removeBid(Controller<_Tp,_Val>* c, _Tp obj)
	{
		if (c == NULL || obj == NULL)
			return false;
		if (bids[obj].contains(c))
		{
			bids[obj].erase(c);
			updatedObjects.insert(obj);
		}
		return true;
	}

	template <class _Tp,class _Val>
	bool Arbitrator<_Tp,_Val>::decline(Controller<_Tp,_Val>* c, _Tp obj, _Val bid)
	{
		if (c == NULL || obj == NULL)
			return false;
		if (bids[obj].top().first != c)
			return false;
		bids[obj].set(c, bid);
		updatedObjects.insert(obj);
		return true;
	}

	template <class _Tp,class _Val>
	bool Arbitrator<_Tp,_Val>::accept(Controller<_Tp,_Val>* c, _Tp obj)
	{
		if (c == NULL || obj == NULL)
			return false;
		if (bids[obj].top().first != c)
			return false;
		if (owner[obj])
		{
			owner[obj]->onRevoke(obj,bids[obj].top().second);
			objects[owner[obj]].erase(obj);
		}
		owner[obj] = c;
		objects[c].insert(obj);
		return true;
	}

	template <class _Tp,class _Val>
	bool Arbitrator<_Tp,_Val>::accept(Controller<_Tp,_Val>* c, _Tp obj, _Val bid)
	{
		if (c == NULL || obj == NULL)
			return false;
		if (bids[obj].top().first != c)
			return false;
		if (owner[obj])
		{
			owner[obj]->onRevoke(obj, bids[obj].top().second);
			objects[owner[obj]].erase(obj);
		}
		bids[obj].set(c,bid);
		owner[obj] = c;
		objects[c].insert(obj);
		updatedObjects.insert(obj);
	}

	template <class _Tp,class _Val>
	bool Arbitrator<_Tp,_Val>::hasBid(_Tp obj) const
	{
		return (bids.find(obj)!=bids.end());
	}

	template <class _Tp,class _Val>
	const std::pair<Controller<_Tp,_Val>*, _Val>& Arbitrator<_Tp,_Val>::getHighestBidder(_Tp obj) const
	{
		return bids.find(obj)->second.top();
	}

	template <class _Tp,class _Val>
	const std::list< std::pair<Controller<_Tp,_Val>*, _Val> > Arbitrator<_Tp,_Val>::getAllBidders(_Tp obj) const
	{
		std::list< std::pair<Controller<_Tp,_Val>*, _Val> > bidders;
		if (bids.find(obj)==bids.end())
			return bidders;
		Heap<Controller<_Tp,_Val>*, _Val> bid_heap=bids.find(obj)->second;
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
		return objects.find(c)->second;
	}

	template <class _Tp,class _Val>
	void Arbitrator<_Tp,_Val>::onRemoveObject(_Tp obj)
	{
		bids.erase(obj);
		owner.erase(obj);
		updatedObjects.erase(obj);
		for(std::map<Controller<_Tp,_Val>*, std::set<_Tp> >::iterator c=objects.begin();c!=objects.end();c++)
		{
			(*c).second.erase(obj);
		}
	}

	template <class _Tp,class _Val>
	_Val Arbitrator<_Tp,_Val>::getBid(Controller<_Tp,_Val>* c, _Tp obj) const
	{
		return bids.find(obj)->second.get(c);
	}

	template <class _Tp,class _Val>
	void Arbitrator<_Tp,_Val>::update()
	{
		std::map<Controller<_Tp,_Val>*, std::set<_Tp> > objectsToOffer;
		for(std::set<_Tp>::iterator i = updatedObjects.begin(); i != updatedObjects.end(); i++)
		{
			if (!bids[*i].empty())
			{
				if (owner.find(*i) == owner.end() || bids[*i].top().first != owner[*i])
					objectsToOffer[bids[*i].top().first].insert(*i);
			}
			else
			{
				if (owner.find(*i) != owner.end())
				{
					_Val temp=0;
					owner.find(*i)->second->onRevoke(*i,temp);
					owner.erase(*i);
				}
			}
		}
		updatedObjects.clear();
		for(std::map< Controller<_Tp,_Val>*, std::set<_Tp> >::iterator i = objectsToOffer.begin(); i != objectsToOffer.end(); i++)
			(*i).first->onOffer((*i).second);
	}

}
