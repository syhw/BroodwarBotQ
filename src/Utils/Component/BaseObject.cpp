#include "BaseObject.h"
#include "ObjectManager.h"
#include <algorithm>

BaseObject::BaseObject(std::string name)
: sout( )
, serr( )
, className(name)
{
    ObjectManager::Instance().addObject(this);
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

void BaseObject::addData(BaseData* data)
{
    vData.push_back(data);
}

const std::vector<BaseData*>& BaseObject::getData() const
{
    return vData;
}

const std::string& BaseObject::getWarnings() const
{
    return outputs;
}

const std::string& BaseObject::getErrors() const
{
    return warnings;
}
