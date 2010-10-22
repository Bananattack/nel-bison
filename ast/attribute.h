#pragma once

#include <sstream>

#include "node.h"
#include "string_node.h"
#include "list_node.h"
#include "definition.h"

namespace nel
{
    class Attribute : public Node
    {
        private:
            ListNode<StringNode*>* pieces;
            
        public:
            Attribute(ListNode<StringNode*>* pieces, SourcePosition* sourcePosition);
            ~Attribute();
            
            
            /**
             * Convert the attribute into a string representation,
             * such as 'a.b.c' or 'foo'
             */
            std::string getName()
            {
                std::ostringstream os;
                
                bool separator = false;
                ListNode<StringNode*>::ListType& list = pieces->getList();
                
                for(size_t i = 0; i < list.size(); i++)
                {
                    if(separator)
                    {
                        os << "." << list[i]->getValue();
                    }
                    else
                    {
                        os << list[i]->getValue();
                        separator = true;
                    }
                    
                }
                return os.str();
            }
            
            /**
             * Returns the list of pieces needed to find the attribute reference.
             * And every piece but the last is a package qualifier, for example:
             * `a.b.c` returns { a, b, c }
             * `a` returns { a }
             */
            ListNode<StringNode*>* getPieces()
            {
                return pieces;
            }
            
            /**
             * Resolve the definition that this attribute refers to.
             * If forbidUndefined is set, then it will error upon missing symbols.
             */
            Definition* findDefinition(bool forbidUndefined);
    };
}