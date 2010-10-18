#include <cstdlib>
#include <iostream>
#include "error.h"

namespace nel
{
    unsigned int errorCount = 0;

    static void printErrorSource(SourcePosition* sourcePosition)
    {
        if(sourcePosition)
        {
            sourcePosition->print(std::cerr);
        }
        else
        {
            std::cerr << "??? (source unknown)";
        }
    }

    static void incrementErrorCount(bool fatal)
    {
		errorCount++;
		if(errorCount >= ERROR_MAX_COUNT)
		{
            fatal = true;
            std::cerr << "  fatal: too many errors (max of " << ERROR_MAX_COUNT << ")." << std::endl;
		}
        if(fatal)
        {
            failAndExit();
        }
    }
    
    void failAndExit()
    {
        std::cerr << "* " << PROGRAM_NAME << ": failed with " << errorCount << " error(s)." << std::endl;
        exit(1);
    }

    void internalError(std::string message, bool fatal)
    {
        std::cerr << "  " << (fatal ? "fatal: " : "") << " internal error: " << message << std::endl;
        incrementErrorCount(fatal);
    }

    void error(std::string message, SourcePosition* sourcePosition, bool fatal)
    {
        std::cerr << "  " << (fatal ? "fatal: " : "");
        printErrorSource(sourcePosition);
        std::cerr << ": " << message << std::endl;
        incrementErrorCount(fatal);
    }
}