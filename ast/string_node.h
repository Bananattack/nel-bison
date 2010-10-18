#pragma once

#include "node.h"

namespace nel
{
    /**
     * A node representing a string token of some kind, such as an identifier or string literal.
     */
    class StringNode : public Node
    {
        private:
            std::string value;
            
        public:
            StringNode(std::string value, SourcePosition* sourcePosition)
                : Node(sourcePosition), value(value)
            {
            }
            
            /**
             * Returns the string value of this node.
             */
            std::string getValue()
            {
                return value;
            }
    };
}