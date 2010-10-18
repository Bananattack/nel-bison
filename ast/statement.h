#pragma once

#include "node.h"

namespace nel
{
    /**
     * A typically stand-alone unit of program code which either
     * affects the compiler's current state, declares something,
     * or emits executable code.
     */
    class Statement : public Node
    {
        public:
            /**
             * An enumeration of all the different kinds of statements available.
             */
            enum StatementType
            {
                BLOCK,                  /**< A compound block statement, used for scoping. */
                HEADER,                 /**< A header statement. */
                INCLUDE,                /**< An inclusion point. */
                RELOCATION,             /**< A relocation statement, used to move the ROM and RAM positions. */
                LABEL_DECLARATION,      /**< A label declaration. */
                CONSTANT_DECLARATION,   /**< A constant declaration. */
                VARAIBLE_DECLARATION,   /**< A variable declaration. */
                DATA,                   /**< A data statement. */
                COMMAND,                /**< A command statement. */
                BRANCH,                 /**< A branching statement, like goto, call or return. */
                EMBED                   /**< A point to embed a binary file. */
            };
            
        private:
            StatementType statementType;
            
        public:
            Statement(StatementType statementType, SourcePosition* sourcePosition)
                : Node(sourcePosition), statementType(statementType)
            {
            }
            
            virtual ~Statement()
            {
            }
            
            /**
             * Returns the type of statement that this node represents.
             */
            StatementType getStatementType()
            {
                return statementType;
            }
            
            /**
             * Gathers general program information and declarations.
             */
            virtual void aggregate() = 0;
            
            /**
             * Basic validation of statements and calculating operation sizes and label positions.
             */
            virtual void validate() = 0;
            
            /**
             * Final validation and code output.
             */
            virtual void generate() = 0;
    };
}