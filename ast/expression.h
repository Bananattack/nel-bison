#pragma once

#include <vector>

#include "node.h"
#include "number_node.h"
#include "string_node.h"
#include "operation.h"
#include "definition.h"

namespace nel
{
    class Operation;
    
    /**
     * An expression is a collection of numbers, identifiers and operations
     * between them, which combine to a single numeric value.
     */
    class Expression : public Node
    {
        public:
            static const unsigned int MAX_VALUE = 65535;
        
            /**
             * An enumeration of all the different kinds of expressions available.
             */
            enum ExpressionType
            {
                NUMBER,         /**< A numeric term. */
                IDENTIFIER,     /**< An identifier, naming a constant or label */
                OPERATION,      /**< An operation to perform. */
            };

        private:
            ExpressionType expressionType;
            
            union
            {
                NumberNode* number;
                StringNode* identifier;
                Operation* operation;
            };
            
            bool folded;
            unsigned int foldedValue;
            
        public:
            Expression(NumberNode* number, SourcePosition* sourcePosition);
            Expression(StringNode* identifier, SourcePosition* sourcePosition);
            Expression(Operation* operation, SourcePosition* sourcePosition);
            ~Expression();
            
        private:
            void init();
            
            bool fold(bool mustFold, bool symbolsMustExist, std::vector<Definition*>& expansionStack);
            
        public:
            /**
             * Returns the kind of expression that this node represents.
             */
            ExpressionType getExpressionType()
            {
                return expressionType;
            }
        
            /**
             * Returns the numeric node associated with a NUMBER expression, and 0 otherwise.
             */
            NumberNode* getNumberNode()
            {
                return expressionType == NUMBER ? number : 0;
            }

            /**
             * Returns the identifier associated with an IDENTIFIER expression, and 0 otherwise.
             */
            StringNode* getIdentifier()
            {
                return expressionType == IDENTIFIER ? identifier : 0;
            }

            /**
             * Returns the operation associated with an OPERATION expression, and 0 otherwise.
             */            
            Operation* getOperation()
            {
                return expressionType == OPERATION ? operation : 0;
            }
            
            /**
             * Whether or not the expression has been successfully folded.
             */
            bool isFolded()
            {
                return folded;
            }
            
            /**
             * The result of folding. Its value is undefined if isFolded() returns false.
             */
            unsigned int getFoldedValue()
            {
                return foldedValue;
            }
            
            /**
             * Attempts to fold together the expression tree underneath this node.
             * If it succeeds, returns true. Otherwise, it returns false.
             */
            bool fold(bool mustFold, bool forbidUndefined);
    };
}