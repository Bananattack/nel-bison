#pragma once

#include "node.h"
#include "expression.h"

namespace nel
{
    class Expression;
    
    /**
     * An operation which joins together two expressions into a different numeric value.
     * These operations are either arithmetic or logical.
     */
    class Operation : public Node
    {
        public:
            /**
             * An enumeration of possible operations that can be performed.
             */
            enum OperationType
            {
                // In order of precedence levels (which will have already been taken care of by the parser):
                // * / %
                MUL,            /**< Multiplication */
                DIV,            /**< Floor Division */
                MOD,            /**< Modulo */    
                // + -
                ADD,            /**< Addition */
                SUB,            /**< Subtraction */
                // << >>
                SHL,            /**< A logical shift left. */
                SHR,            /**< A logical shift right. */
                // &
                BITWISE_AND,    /**< A bitwise AND operation. */
                // ^
                BITWISE_XOR,    /**< A bitwise XOR operation. */
                // |
                BITWISE_OR      /**< A bitwise OR operation. */
            };
        private:
            OperationType operationType;
            Expression* left;
            Expression* right;
        public:
            Operation(OperationType operationType, Expression* left, Expression* right, SourcePosition* sourcePosition);
            ~Operation();
            
            /**
             * Gets the type of operation to perform.
             */
            OperationType getOperationType()
            {
                return operationType;
            }
            
            /**
             * Gets the left operand of the operation.
             */
            Expression* getLeft()
            {
                return left;
            }

            /**
             * Gets the right operand of the operation.
             */            
            Expression* getRight()
            {
                return right;
            }
    };
}