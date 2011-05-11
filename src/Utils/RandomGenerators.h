#pragma once
#include <CSingleton.h>
#include <ctime>
#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

typedef boost::minstd_rand base_generator_type; 
// can try also boost::mt19937 or boost::ecuyer1988
typedef boost::variate_generator<base_generator_type&, boost::uniform_real<> >
	uniform_generator;

template <int _left, int _right>
class RandomGenerators : public CSingleton<RandomGenerators<_left, _right> >
{
	friend class CSingleton<RandomGenerators<_left, _right> >;
private:
    base_generator_type generator;
	uniform_generator uni_real_generator;
    RandomGenerators()
		: generator(base_generator_type(static_cast<unsigned int>(std::time(0))))
		, uni_real_generator(uniform_generator(generator, boost::uniform_real<>
		(_left, _right)))
	{ }
	~RandomGenerators() { }
public:
	double uni() { return uni_real_generator(); }
};