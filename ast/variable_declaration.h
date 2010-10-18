#pragma once

#include "statement.h"
#include "string_node.h"
#include "list_node.h"
#include "expression.h"

namespace nel
{
    /**
     * A declaration of one or more variables of a supplied type.
     * It reserves space in RAM for this usage.
     */
    class VariableDeclaration : public Statement
    {   
        public:
            /**
             * An enumeration of possible base types which can be used for variables.
             */
            enum VariableType
            {
                BYTE,   /**< A single byte unit. */
                WORD    /**< A two-byte unit. */
            };
        
        private:
            VariableType variableType;
            ListNode<StringNode*>* names;
            Expression* arraySizeExpression;
            
        public:    
            VariableDeclaration(VariableType variableType, ListNode<StringNode*>* names, SourcePosition* sourcePosition);
            VariableDeclaration(VariableType variableType, ListNode<StringNode*>* names, Expression* arraySizeExpression, SourcePosition* sourcePosition);
            ~VariableDeclaration();
            
            /**
             * Gets the base type of the variable being declared.
             */
            VariableType getVariableType()
            {
                return variableType;
            }

            /**
             * Returns a list of variable names which were supplied to this declaration.
             */            
            ListNode<StringNode*>* getNames()
            {
                return names;
            }
            
            /**
             * Returns an expression associated with the array size of each variable,
             * or 0 if there is nothing (in which case, it's a single unit).
             */
            Expression* getArraySizeExpression()
            {
                return arraySizeExpression;
            }

            void aggregate();
            void validate();
            void generate();
    };
}