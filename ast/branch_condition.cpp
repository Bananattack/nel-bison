#include "branch_condition.h"

namespace nel
{
    BranchCondition::BranchCondition(ConditionType conditionType, Argument* flag, SourcePosition* sourcePosition)
        : Node(sourcePosition), conditionType(conditionType), flag(flag)
    {
    }
    
    BranchCondition::~BranchCondition()
    {
        delete flag;
    }
}