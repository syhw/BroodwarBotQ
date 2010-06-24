#pragma once
#include <CSingleton.h>
#include <list>
#include <set>
#include <map>
#include "Heap.h"
#include "Controller.h"
namespace Arbitrator
{
	template <class _Tp,class _Val>
	class Arbitrator: public CSingleton<Arbitrator<_Tp, _Val> >
	{
		friend class CSingleton<Arbitrator<_Tp, _Val> >;

	public:
		bool setBid(Controller<_Tp,_Val>* c, _Tp obj, _Val bid);
		bool removeBid(Controller<_Tp,_Val>* c, _Tp obj);
		bool accept(Controller<_Tp,_Val>* c, _Tp obj, _Val bid);
		bool accept(Controller<_Tp,_Val>* c, _Tp obj);
		bool decline(Controller<_Tp,_Val>* c, _Tp obj, _Val bid);
		bool hasBid(_Tp obj) const;
		const std::pair<Controller<_Tp,_Val>*, _Val>& getHighestBidder(_Tp obj) const;
		const std::list< std::pair<Controller<_Tp,_Val>*, _Val> > getAllBidders(_Tp obj) const;
		const std::set<_Tp>& getObjects(Controller<_Tp,_Val>* c) const;
		void onRemoveObject(_Tp obj);
		_Val getBid(Controller<_Tp,_Val>* c, _Tp obj) const;
		void update();
	private:
		std::map<_Tp,Heap<Controller<_Tp,_Val>*, _Val> > bids;
		std::map<_Tp,Controller<_Tp,_Val>* > owner;
		std::map<Controller<_Tp,_Val>*, std::set<_Tp> > objects;
		std::set<_Tp> updatedObjects;

		Arbitrator(){};
		~Arbitrator(){};
	};

}

#include "Arbitrator.inl"
