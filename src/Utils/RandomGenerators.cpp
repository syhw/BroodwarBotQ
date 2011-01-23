#include "RandomGenerators.h"

RandomGenerators::RandomGenerators()
: generator(base_generator_type(static_cast<unsigned int>(std::time(0))))
, uni_real(boost::uniform_real<>(0, 1))
, uni_real_generator(boost::variate_generator
					 <base_generator_type&, boost::uniform_real<> >(generator, uni_real))
{
}

RandomGenerators::~RandomGenerators()
{
}

double RandomGenerators::uni_0_1()
{
    return uni_real_generator();
}



