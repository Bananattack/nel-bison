#include "source_position.h"
#include "source_file.h"
   
namespace nel
{
    SourceFile::SourceFile(std::string filename, SourcePosition* includePoint)
        : filename(filename), includePoint(includePoint)
    {
    }
    
    SourceFile::SourceFile(SourceFile* sourceFile)
        : filename(sourceFile->filename), includePoint(sourceFile->includePoint)
    {
    }
    
    SourceFile::~SourceFile()
    {
        delete includePoint;
    }
}