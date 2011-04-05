#include <PrecompiledHeader.h>
#include "BaseObject.h"
#include "ObjectManager.h"

BaseObject::BaseObject(std::string name)
: className(name)
{
    ObjectManager::Instance().addObject(this);
}

BaseObject::~BaseObject()
{
}

void BaseObject::onFrame()
{
	update();
}

void BaseObject::display() const
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
		std::cerr<< "ERROR[" << getClassName() << "]: "<<serr.str();
		BWAPI::Broodwar->printf( "[%s]: %s", getClassName().c_str(), serr.str().c_str());
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
