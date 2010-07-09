#pragma once
#include <vector>
#include <string>
#include <iostream>
#include "CustomOStream.h"
#include "Defines.h"
#include <QtGui/QWidget>
#include <QtGui/QMessageBox>
#include "RefreshWidgetEmiter.h"
#include "time.h"
#include "BWAPI.h"

class BaseData;

class BaseObject
{
public:
		BaseObject(std::string name);
    ~BaseObject();

		void update();
		virtual void onFrame() = 0;

		// Qt widget interface
		virtual QWidget* createWidget(QWidget* parent) const; // must return the new widget with given parent
		virtual void refreshWidget(QWidget* widget) const = 0; // update the given widget wich was returned by createWidget(QWidget* parent)

public:
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
