#pragma once
#include <exception>
#include "Utils/Utils/TimeManager.h"


/** Autoregister itself to the TimeManager singleton
*/
struct TimeManaged
{
    TimeManaged()
    {
        TimeManager* tm = & TimeManager::Instance();
        tm->insert(this);
    }
};
