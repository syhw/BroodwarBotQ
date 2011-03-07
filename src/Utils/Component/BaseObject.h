#pragma once
#include <vector>
#include <string>
#include <iostream>
#include "CustomOStream.h"
#include "Defines.h"
#include "time.h"
#include <BWAPI.h>

class BaseData;

class BaseObject
{
public:
    BaseObject(std::string name);
    ~BaseObject();

    void onFrame();
    virtual void update() = 0;

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

protected:
    std::vector<BaseData*> vData;
    std::string warnings;
    std::string outputs;
    std::string className;
};
