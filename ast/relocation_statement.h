#pragma once

#include "statement.h"
#include "expression.h"

namespace nel
{
    /**
     * A relocation statement which affects the RAM or ROM positioning.
     */
    class RelocationStatement : public Statement
    {
        public:
            /**
             * An enumeration of the different things that can be relocated.
             */
            enum RelocationType
            {
                ROM,    /**< Affects the active ROM bank and/or the position of the program counter. */
                RAM,    /**< Affects the position of the internal RAM counter. */
            };
            
        private:
            RelocationType relocationType;
        
            Expression* bankExpression;
            Expression* destinationExpression;
            
        public:
            RelocationStatement(RelocationType relocationType, Expression* destinationExpression, SourcePosition* sourcePosition);
            RelocationStatement(RelocationType relocationType, Expression* bankExpression, Expression* destinationExpression, SourcePosition* sourcePosition);            
            ~RelocationStatement();
            
            /**
             * Returns the type of relocation that this node represents.
             */
            RelocationType getRelocationType()
            {
                return relocationType;
            }
            
            /**
             * Returns the bank that this relocation is switching to, or 0 if it is unaffected.
             */
            Expression* getBankExpression()
            {
                return bankExpression;
            }
            
            /**
             * A destination address to relocate the program/RAM counter, or 0 if it is unaffected.
             */
            Expression* getDestinationExpression()
            {
                return destinationExpression;
            }

            void aggregate();
            void validate();
            void generate();
    };
}