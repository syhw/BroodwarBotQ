#pragma once
#include <stdlib.h> 
#include <BWAPI.h>

struct Rainbow {
    std::vector<BWAPI::Color> rainbow;
    unsigned int ind;
    Rainbow() 
    {
        rainbow.clear();
        rainbow.push_back(BWAPI::Color(244, 0, 0)); // red (R,G,B)
        rainbow.push_back(BWAPI::Color(244, 42, 42));
        rainbow.push_back(BWAPI::Color(244, 92, 92));
        rainbow.push_back(BWAPI::Color(244, 128, 128));
        // rainbow.push_back(BWAPI::Color(244, 92, 92));
    }
    Rainbow(BWAPI::Color startColor, int increment)
    {
        rainbow.clear();
        int primaryred = startColor.red();
        int primarygreen = startColor.green();
        int primaryblue = startColor.blue();
        int secondaryred = (primaryred >= primarygreen 
            && primaryred >= primaryblue) ? 0 : 1;
        int secondarygreen = (primarygreen >= primaryblue
            && primarygreen > primaryred) ? 0 : 1;
        int secondaryblue = (primaryblue > primaryred
            && primaryblue > primarygreen) ? 0 : 1;
        int maxsecondary = max(min(primaryred, primarygreen), min(primaryred, primaryblue));
        int i = 0;
        while ((maxsecondary + i) < 255)
        {
            rainbow.push_back(BWAPI::Color(primaryred + secondaryred*i,
                primarygreen + secondarygreen*i,
                primaryblue + secondaryblue*i));
            i += increment;
        }
    }
    ~Rainbow() {}
    BWAPI::Color nextColor()
    {
        return rainbow[ind++ % rainbow.size()];
    }
};
