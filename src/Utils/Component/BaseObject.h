#pragma once
#include <vector>
#include <string>
#include <iostream>
#include "CustomOStream.h"
#include "Defines.h"
#ifdef BW_QT_DEBUG
#include <QtGui/QWidget>
#include <QtGui/QMessageBox>
#include "RefreshWidgetEmiter.h"
#endif
#include "time.h"
#include "BWAPI.h"

class BaseData;

class BaseObject
{
public:
    BaseObject(std::string name);
    ~BaseObject();

    void onFrame();
    virtual void update() = 0;

#ifdef BW_QT_DEBUG
    // Qt widget interface
    virtual QWidget* createWidget(QWidget* parent) const; // must return the new widget with given parent
    virtual void refreshWidget(QWidget* widget) const = 0; // update the given widget wich was returned by createWidget(QWidget* parent)
#endif

    // Minimap display
    virtual void display() const; // Each baseObject can display informations on the minimap by overloading this method

    std::string getClassName() const;
    void processStream(std::ostream& out);
    void addData(BaseData* data);
    const std::vector<BaseData*>& getData() const;
    const std::string& getWarnings() const;
    const std::string& getErrors() const;

    mutable CustomOStream<BaseObject> sendl;
    mutable std::ostringstream sout;
    mutable std::ostringstream serr;

    // Profiling
    clock_t beginTime, endTime, timeEllapsed;

#ifdef BW_QT_DEBUG
    RefreshWidgetEmiter* refreshWidgetEmiter;
#endif

protected:
    std::vector<BaseData*> vData;
    std::string warnings;
    std::string outputs;
    std::string className;
};
