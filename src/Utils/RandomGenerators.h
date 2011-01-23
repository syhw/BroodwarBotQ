#pragma once
#include <CSingleton.h>
#include <ctime>
#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

typedef boost::minstd_rand base_generator_type; 
// can try also boost::mt19937 or boost::ecuyer1988

class RandomGenerators : public CSingleton<RandomGenerators>
{
	friend class CSingleton<RandomGenerators>;
private:
    base_generator_type generator;
    boost::uniform_real<> uni_real;
    boost::variate_generator<base_generator_type&, boost::uniform_real<> > uni_real_generator;
    RandomGenerators();
    ~RandomGenerators();
public:
    double uni_0_1();
};

