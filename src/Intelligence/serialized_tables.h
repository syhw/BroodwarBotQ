#pragma once

#include "boost/archive/text_oarchive.hpp"
#include "boost/archive/text_iarchive.hpp"
#include "boost/serialization/vector.hpp"
#include "boost/serialization/set.hpp"

struct serialized_tables
{
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & tabulated_P_Time_X_Op;
        ar & tabulated_P_X_Op;
        ar & openings;
        ar & vector_X;
        ar & distances_X;
    }
	std::vector<long double> tabulated_P_Time_X_Op;
    std::vector<long double> tabulated_P_X_Op;
    std::vector<std::string> openings;
    std::vector<std::set<int> > vector_X;
    std::vector<std::vector<int> > distances_X;
    serialized_tables() {};
    serialized_tables(const std::vector<long double>& time_x,
            const std::vector<long double>& x,
            const std::vector<std::string>& op,
            const std::vector<std::set<int> >& vx,
            const std::vector<std::vector<int> >& dx)
        : tabulated_P_Time_X_Op(time_x)
        , tabulated_P_X_Op(x)
        , openings(op)
        , vector_X(vx)
        , distances_X(dx)
    {}
	void swap(serialized_tables& st)
	{
		tabulated_P_Time_X_Op.swap(st.tabulated_P_Time_X_Op);
		tabulated_P_X_Op.swap(st.tabulated_P_X_Op);
		openings.swap(st.openings);
		vector_X.swap(st.vector_X);
		distances_X.swap(st.distances_X);
	}
};


struct openings_knowing_player
{
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
		ar & numberGames; // against this player
        ar & tabulated_P_Op_knowing_Time; // against this player
    }
	std::vector<int> numberGames; // number of games/time against player
	std::vector<std::vector<long double> > tabulated_P_Op_knowing_Time; 
	// in minutes, time (first parameter) and player (the serialized file)
	// specific: tabulated_P_Op_knowing[time in minute] 
	// is a player specific prior
    openings_knowing_player() {}
    openings_knowing_player(int nb,
			const std::vector<std::vector<long double> >& p_Op)
        : numberGames(nb)
        , tabulated_P_Op_knowing_Time(p_Op)
    {}
	void swap(openings_knowing_player& op)
	{
		tabulated_P_Op_knowing_Time.swap(op.tabulated_P_Op_knowing_Time);
		numberGames.swap(op.numberGames);
	}
};

BOOST_CLASS_TRACKING(openings_knowing_player, boost::serialization::track_never)
