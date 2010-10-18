#pragma once

#include "node.h"
#include "expression.h"

namespace nel
{
    /**
     * A header setting, which goes into a header statement.
     * It consists of recognized name being assigned to an expression.
     */
    class HeaderSetting : public Node
    {
        private:
            StringNode* name;
            Expression* expression;
            
        public:    
            HeaderSetting(StringNode* name, Expression* expression, SourcePosition* sourcePosition);
            ~HeaderSetting();
            
            /**
             * Returns the name of thing being assigned by this header setting.
             */
            StringNode* getName()
            {
                return name;
            }
            
            /**
             * Returns the expression to assign.
             */
            Expression* getExpression()
            {
                return expression;
            }
            
            /**
             * Checks if a setting's value could be folded, and is acceptable.
             * Returns true when successful, and returns false and errors when invalid.
             */
            bool checkValue(unsigned int min, unsigned int max);
    };
}