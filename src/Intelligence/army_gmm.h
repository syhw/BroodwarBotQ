#include <vector>
#include <string>
#include "boost/archive/text_oarchive.hpp"
#include "boost/archive/text_iarchive.hpp"
#include "boost/serialization/vector.hpp"

struct army_gmm
{
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & order; // unit types ordering in the subsequent vectors 
        ar & mu; // #component x #features (unit types)
        ar & sigma; // #components x #features x #features
    }
    std::vector<std::string> order;
    std::vector<std::vector<long double> > mu;
    std::vector<std::vector<std::vector<long double> > > sigma;
};
