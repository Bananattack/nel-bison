#pragma once

#include <vector>

#include "node.h"   

namespace nel
{
    /**
     * A node representing a list of child nodes.
     * The underlying structure is a contiguous list of items.
     */
    template <typename T>
    class ListNode : public Node
    {
        public:
            typedef std::vector<T> ListType;
        private:
            ListType list;
            
        public:
            ListNode(SourcePosition* sourcePosition)
                : Node(sourcePosition)
            {
            }
            
            ListNode(T firstItem, SourcePosition* sourcePosition)
                : Node(sourcePosition)
            {
                list.push_back(firstItem);
            }
            
            ~ListNode()
            {
                for(size_t i = 0; i < list.size(); i++)
                {
                    delete list[i];
                }
            }
            
            /**
             * Returns the underlying list contained by this node.
             */
            ListType& getList()
            {
                return list;
            }
    };
}