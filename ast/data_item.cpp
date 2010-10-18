#include "error.h"
#include "data_item.h"

namespace nel
{
    DataItem::DataItem(Expression* expression, SourcePosition* sourcePosition)
        : Node(sourcePosition), itemType(EXPRESSION), expression(expression)
    {
    }
    
    DataItem::DataItem(StringNode* literal, SourcePosition* sourcePosition)
        : Node(sourcePosition), itemType(STRING_LITERAL), stringLiteral(stringLiteral)
    {
    }
    
    DataItem::~DataItem()
    {
        switch(itemType)
        {
            case STRING_LITERAL:
                delete expression;
                break;
            case EXPRESSION:
                delete stringLiteral;
                break;
        }
    }
    
    void DataItem::check()
    {
        switch(itemType)
        {
            case STRING_LITERAL:
                break;
            case EXPRESSION:
                // Check if the expression uses defined symbols.
                // Don't fret if the value is unknown still.
                expression->fold(false, true);
                break;
        }
    }
}