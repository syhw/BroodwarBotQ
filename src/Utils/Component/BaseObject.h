#pragma once
#include <vector>
#include <iostream>
#include "CustomOStream.h"

class BaseData;


class BaseObject
{
public:
    BaseObject();
    ~BaseObject();

    std::string getClassName() const;
    void processStream(std::ostream& out);

    // Serializer les data pour echanger entre threads.

    mutable CustomOStream<BaseObject> sendl;
    mutable std::ostringstream sout;
    mutable std::ostringstream serr;

protected:
    std::vector<BaseData*> vData;
    std::string warnings;
    std::string outputs;
    std::string className;
};
