#pragma once 

#include "source_position.h"

namespace nel
{
    static const char* const PROGRAM_NAME = "nel";
    const unsigned int ERROR_MAX_COUNT = 30;
    extern unsigned int errorCount;
    
    /**
     * Print failure message and quit the program.
     */
    void failAndExit();
    
    /**
     * Raises an internal error message which cannot be linked to a line in source.
     */
    void internalError(std::string message, bool fatal = true);
    
    /**
     * Raises an error message due to a problem with lexing, syntax,
     * or the semantics of the user's code.
     */
    void error(std::string message, SourcePosition* sourcePosition, bool fatal = false);
}