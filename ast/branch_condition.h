#pragma once

#include "node.h"
#include "argument.h"

namespace nel
{
    /**
     * A condition which affects whether or not a branching construct (ie. goto) is fired.
     */
    class BranchCondition : public Node
    {   
        public:
            /**
             * An enumeration of conditions.
             */
            enum ConditionType
            {
                CONDITION_SET,      /**< True when the condition flag is set. */
                CONDITION_UNSET,    /**< True when the condition flag is unset. */
            };
        
        private:
            ConditionType conditionType;
            Argument* flag;
            
        public:
            BranchCondition(ConditionType conditionType, Argument* flag, SourcePosition* sourcePosition);
            ~BranchCondition();
            
            /**
             * Returns the type of condition associated with this node.
             */
            ConditionType getConditionType()
            {
                return conditionType;
            }
            
            /**
             * Returns the p-flag argument used by this condition.
             */
            Argument* getFlag()
            {
                return flag;
            }
    };
}