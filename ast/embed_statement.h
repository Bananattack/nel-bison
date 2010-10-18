#pragma once

#include "statement.h"
#include "list_node.h"
#include "data_item.h"

namespace nel
{
    /**
     * A statement which indicates a binary file to write verbatim into the ROM.
     */
    class EmbedStatement : public Statement
    {
        private:
            StringNode* filename;
            unsigned int filesize;
            
        public:    
            EmbedStatement(StringNode* filename, SourcePosition* sourcePosition);
            ~EmbedStatement();
            
            /**
             * Returns the name of the file that is being embedded.
             */
            StringNode* getFilename()
            {
                return filename;
            }

            void aggregate();
            void validate();
            void generate();
    };
}