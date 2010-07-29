#pragma once
#include <set>
#include <string>
namespace Arbitrator
{
  template <class _Tp,class _Val> 
  class Controller
  {
  public:
    virtual void onOffer(std::set<_Tp> objects)=0;
    virtual void onRevoke(_Tp, _Val bid)=0;
    virtual std::string getName() const=0;
    virtual std::string getShortName() const
    {
      if (getName().length() >= 3)
        return getName().substr(0, 3);
      else
        return "???";
    }
    virtual void update()=0;
  };
}