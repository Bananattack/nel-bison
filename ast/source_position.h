#pragma once

#include <iostream>

#include "source_file.h"

namespace nel
{
    /**
     * The source position, used to give locational information to
     * abstract syntax nodes, like statements and expressions.
     * This is in turn useful for error reporting behaviour.
     */
    class SourcePosition
    {
        private:
            SourceFile* sourceFile;
            unsigned int line;
            unsigned int column;            
        public:
            SourcePosition(SourceFile* sourceFile);
            SourcePosition(SourcePosition* position);
            ~SourcePosition();
        
            /**
             * Gets the source file that this position has.
             */
            SourceFile* getSourceFile()
            {
                return sourceFile;
            }

            /**
             * Gets the line in the source file.
             */
            unsigned int getLine()
            {
                return line;
            }

            /**
             * Increments the column position on the line.
             */
            void incrementLine(unsigned int amount = 1)
            {
                column = 1;
                line += amount;
            }
            
            /**
             * Gets the column in the line of the source.
             */
            unsigned int getColumn()
            {
                return column;
            }
            
            /**
             * Increments the column position on the line.
             */
            void incrementColumn(unsigned int amount = 1)
            {
                column += amount;
            }
            
            void print(std::ostream& output, bool verbose = false);
    };
}
