#include "BaseObject.h"
#include <algorithm>


BaseObject::BaseObject(BaseObject* parent)
:	parent(parent)
,sout( )
, serr( )
{
    parent->addChild(this);
}

BaseObject::~BaseObject()
{
    for (std::vector<BaseObject*>::iterator it = children.begin(); it != children.end(); ++it)
    {
        (*it)->parent = NULL;
    }
    parent->removeChild(this);
}


std::string BaseObject::getClassName() const
{
	return className;
}

void BaseObject::addChild(BaseObject* p)
{
    children.push_back(p);
}

void BaseObject::removeChild(BaseObject* p)
{
    children.erase(find(children.begin(), children.end(), p));
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
