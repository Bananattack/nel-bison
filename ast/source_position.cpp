#include "source_file.h"
#include "source_position.h"

namespace nel
{
    SourcePosition::SourcePosition(SourceFile* sourceFile)
        : sourceFile(sourceFile), line(1), column(1)
    {
    }
    
    SourcePosition::SourcePosition(SourcePosition* position)
        : sourceFile(new SourceFile(position->sourceFile)), line(position->line), column(position->column)
    {
    }
    
    SourcePosition::~SourcePosition()
    {
        delete sourceFile;
    }

    void SourcePosition::print(std::ostream& output, bool verbose)
    {
        output << sourceFile->getFilename();
        if(verbose && sourceFile->getIncludePoint())
        {
            output << "(included by ";
            // Print the included file without any further verbosity.
            sourceFile->getIncludePoint()->print(output, false);
            output << ")";
        }
        output << ":" << line << "[" << column << "]";
    }
}