#pragma once
#include <exception>
#include "TimeManager.h"


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
