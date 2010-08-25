#pragma once
#include <sstream>
#include <iostream>


template<class Container>
class CustomOStream
{
protected:
	Container* parent;
public:

	friend inline std::ostream &operator << (std::ostream& out, CustomOStream<Container> & s)
	{
		if (s.parent)
			s.parent->processStream(out);
		else out << std::endl;
		return out;
	}

	CustomOStream()
		: parent(NULL)
	{
	}

	~CustomOStream()
	{
	}

	void setParent(Container* p)
	{
		parent = p;
	}

};
