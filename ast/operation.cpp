#include "operation.h"

namespace nel
{
    Operation::Operation(OperationType operationType, Expression* left, Expression* right, SourcePosition* sourcePosition)
        : Node(sourcePosition), operationType(operationType), left(left), right(right)
    {
    }
    
    Operation::~Operation()
    {
        delete left;
        delete right;
    }
}