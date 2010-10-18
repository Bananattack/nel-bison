#pragma once

#include "statement.h"
#include "string_node.h"
#include "expression.h"

namespace nel
{
    /**
     * A declaration of a named constant expression.
     * Its expression is expanded where it is used.
     */
    class ConstantDeclaration : public Statement
    {
        private:
            StringNode* name;
            Expression* expression;
            
        public:    
            ConstantDeclaration(StringNode* name, Expression* expression, SourcePosition* sourcePosition);            
            ~ConstantDeclaration();
            
            /**
             * Returns the name of the constant being defined.
             */
            StringNode* getName()
            {
                return name;
            }

            /**
             * Returns the expression to bind to this constant.
             */            
            Expression* getExpression()
            {
                return expression;
            }

            void aggregate();
            void validate();
            void generate();
    };
}