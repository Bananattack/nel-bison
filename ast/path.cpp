#include "path.h"

namespace nel
{
    // ifdef and blah blah for platform independence
    #ifdef NEL_WIN32
        const char* delimiters = "\\/";
    #else // assume Unix
        const char* delimiters = "/";
    #endif

    std::string getDirectory(const std::string& s)
    {
        unsigned int p = s.find_last_of(delimiters);
        if (p == std::string::npos)
        {
            return "";
        }

        return s.substr(0, p + 1);
    }   
}