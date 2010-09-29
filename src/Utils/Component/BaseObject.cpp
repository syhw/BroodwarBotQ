#include "BaseObject.h"
#include "ObjectManager.h"
#include <algorithm>

BaseObject::BaseObject(std::string name)
: className(name)
{
    ObjectManager::Instance().addObject(this);
#ifdef BW_QT_DEBUG
    refreshWidgetEmiter = new RefreshWidgetEmiter();
#endif
}

BaseObject::~BaseObject()
{
}

void BaseObject::onFrame()
{
#ifdef BW_QT_DEBUG
	// Profile update of the component
	beginTime = clock();

	update();

	endTime = clock();
	timeEllapsed = (endTime - beginTime) / CLOCKS_PER_SEC;

	// refresh QtWidget all half seconds
	if (!(clock()%12))
		refreshWidgetEmiter->refresh(); // maybe replace this call by a timer should be better ?

	if (!sout.str().empty()) processStream(sout);
	if (!serr.str().empty()) processStream(serr);
#else
	update();
#endif
}

#ifdef BW_QT_DEBUG
QWidget* BaseObject::createWidget(QWidget* parent) const
{
	return new QLabel(QString("createWidget and refreshWidget undefined for this component."), parent);
}
#endif

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
