#include "BaseObject.h"

BaseObject::BaseObject()
:	sout( )
, serr( )
{

}

BaseObject::~BaseObject()
{

}


std::string BaseObject::getClassName() const
{
	return className;
}

void BaseObject::processStream(std::ostream& out)
{
	if (&out == &serr)
	{	    
		serr << "\n";
		std::cerr<< "WARNING[" << getClassName() << "]: "<<serr.str();
		warnings += serr.str();
		serr.str("");
	}
	else if (&out == &sout)
	{
		sout << "\n";
		outputs += sout.str();
		sout.str("");
	}
}
