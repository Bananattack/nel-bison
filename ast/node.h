#pragma once

#include <string>
#include <iostream>

#include "source_position.h"

namespace nel
{
    class SourcePosition;

    /**
     * An abstract node from which all other nodes are defined.
     */
    class Node
    {
        private:
            SourcePosition* sourcePosition;
            
        public:
            Node(SourcePosition* sourcePosition)
                : sourcePosition(sourcePosition)
            {
            }
            
            virtual ~Node()
            {
                delete sourcePosition;
            }
            
            /**
             * Returns the approximate position in source where this node was defined.
             */
            virtual SourcePosition* getSourcePosition()
            {
                return sourcePosition;
            }
    };
}