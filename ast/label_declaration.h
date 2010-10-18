#pragma once

#include "statement.h"
#include "string_node.h"

namespace nel
{
    class LabelDefinition;
    
    /**
     * A declaration of a label, which is used as a positional marker in code.
     * These are typically used to label a chunk of data or executable statement.
     */
    class LabelDeclaration : public Statement
    {
        private:
            StringNode* name;
            LabelDefinition* definition;
            
        public:    
            LabelDeclaration(StringNode* name, SourcePosition* sourcePosition);
            ~LabelDeclaration();
            
            /**
             * Returns the name of the label being defined.
             */
            StringNode* getName()
            {
                return name;
            }

            void aggregate();
            void validate();
            void generate();
    };
}