#include <sstream>
#include "error.h"
#include "header_setting.h"

namespace nel
{
    HeaderSetting::HeaderSetting(StringNode* name, Expression* expression, SourcePosition* sourcePosition)
        : Node(sourcePosition), name(name), expression(expression)
    {
    }
    
    HeaderSetting::~HeaderSetting()
    {
        delete name;
        delete expression;
    }
    
    bool HeaderSetting::checkValue(unsigned int min, unsigned int max)
    {
        if(!expression->fold(true, false))
        {
            std::ostringstream os;
            os << "ines header has `" << name->getValue() << "` setting with a value which could not be resolved.";
            error(os.str(), getSourcePosition());
            return false;
        }
        else if(expression->getFoldedValue() < min || expression->getFoldedValue() > max)
        {
            std::ostringstream os;
            os << "ines header's `" << name->getValue() << "` setting must be between " <<
                min << ".." << max << ", but got " << expression->getFoldedValue() << " instead.";
            error(os.str(), getSourcePosition());
            return false;
        }
        return true;
    }
}