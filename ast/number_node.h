#pragma once

#include "node.h"

namespace nel
{
    /**
     * A node representing a unsigned integer value.
     */
    class NumberNode : public Node
    {
        private:
            unsigned int value;
            
        public:
            NumberNode(unsigned int value, SourcePosition* sourcePosition)
                : Node(sourcePosition), value(value)
            {
            }
            
            /**
             * Returns the numeric value associated with this node.
             */
            unsigned int getValue()
            {
                return value;
            }
    };
}