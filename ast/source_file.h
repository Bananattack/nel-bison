#pragma once

#include <string>

namespace nel
{
    class SourcePosition;
    
    /**
     * Information about a file contained in the source.
     * This file was either the argument to the compiler, or an inclusion made within source code.
     */
    class SourceFile
    {
        private:
            std::string filename;
            SourcePosition* includePoint;
            
        public:
            SourceFile(std::string filename, SourcePosition* includePoint = 0);
            SourceFile(SourceFile* sourceFile);
            ~SourceFile();
            
            /**
             * Returns the filename of this source file.
             */
            std::string getFilename()
            {
                return filename;
            }
            
            /**
             * Returns the point at which this file was included, or 0 if it was passed on the commandline.
             */
            SourcePosition* getIncludePoint()
            {
                return includePoint;
            }
    };
}