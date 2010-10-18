#pragma once

#include "statement.h"
#include "branch_condition.h"
#include "argument.h"

namespace nel
{
    /**
     * A branching construct, that alters the flow of the program.
     * The goto, call, and return statements are examples of this.
     */
    class BranchStatement : public Statement
    {   
        public:
            /**
             * An enumeration of all the different kinds of branch constructs within the language.
             */
            enum BranchType
            {
                NOP,        /**< Do nothing. */
                GOTO,       /**< Branch (possibly with condition). */
                CALL,       /**< Function Call. */
                RETURN,     /**< Return from subroutine.*/
                RTI         /**< Return from interrupt. */
            };
        
        private:
            BranchType branchType;
            Argument* destination;
            BranchCondition* condition;
            
        public:
            BranchStatement(BranchType branchType, SourcePosition* sourcePosition);
            BranchStatement(BranchType branchType, Argument* destination, SourcePosition* sourcePosition);
            BranchStatement(BranchType branchType, Argument* destination, BranchCondition* condition, SourcePosition* sourcePosition);
            ~BranchStatement();

            /**
             * Returns the kind of branch this statement represents.
             */
            BranchType getBranchType()
            {
                return branchType;
            }

            /**
             * Returns the destination of the branch, or 0 when the destination is implicit.
             */
            Argument* getDestination()
            {
                return destination;
            }

            /**
             * Returns the condition under which this branch should occur, or 0 if there is none.
             */
            BranchCondition* getCondition()
            {
                return condition;
            }

            void aggregate();
            void validate();
            void generate();
    };
}