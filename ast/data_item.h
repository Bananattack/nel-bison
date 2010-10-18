#pragma once

#include "node.h"
#include "string_node.h"
#include "expression.h"

namespace nel
{
    /**
     * An item within a data statement's list, which may
     * be either a constant expression or string literal.
     */
    class DataItem : public Node
    {
        public:
            /**
             * An enumeration of all possible types which can appear
             * inside a data statement's list item. 
             */
            enum ItemType
            {
                EXPRESSION,         /**< A single numeric unit. */
                STRING_LITERAL      /**< A string literal comprised of 0 or more characters. */
            };
            
        private:
            ItemType itemType;
            
            union
            {
                Expression* expression;
                StringNode* stringLiteral;
            };
            
        public:
            DataItem(Expression* expression, SourcePosition* sourcePosition);
            DataItem(StringNode* literal, SourcePosition* sourcePosition);
            ~DataItem();
            
            /**
             * Returns the type of item this node represents.
             */
            ItemType getItemType()
            {
                return itemType;
            }

            /**
             * Returns the expression associated with an EXPRESSION item, and 0 otherwise.
             */
            Expression* getExpression()
            {
                return (itemType == EXPRESSION) ? expression : 0;
            }

            /**
             * Returns the string associated with a STRING_LITERAL item, and 0 otherwise.
             */            
            StringNode* getStringLiteral()
            {
                return (itemType == STRING_LITERAL) ? stringLiteral : 0;
            }
            
            /**
             * Ensure that this item is defined (as for the value, it will possibly not yet be known).
             */
            void check();
    };
}