#include <sstream>
#include <iomanip>
#include "error.h"
#include "definition.h"
#include "symbol_table.h"
#include "argument.h"

namespace nel
{
    Argument::ArgumentType Argument::resolveUnprefixedBuiltinType(const std::string& name)
    {
        Definition* definition = SymbolTable::getBuiltins()->tryGet(name);
        
        if(definition)
        {
            switch(definition->getDefinitionType())
            {
                case Definition::A:
                    return A;
                case Definition::X:
                    return X;
                case Definition::Y:
                    return Y;
                case Definition::S:
                    return S;
                case Definition::P:
                    return P;
                case Definition::CARRY:
                    return CARRY;
                case Definition::INTERRUPT:
                    return INTERRUPT;
                case Definition::DECIMAL:
                    return DECIMAL;
                case Definition::OVERFLOW:
                    return OVERFLOW;
                case Definition::ZERO:
                    return ZERO;
                case Definition::NEGATIVE:
                    return NEGATIVE;
            }
        }
        return INVALID;
    }
    
    Argument::ArgumentType Argument::resolveConditionType(const std::string& name)
    {
        Definition* definition = SymbolTable::getBuiltins()->tryGet(name);
        
        if(definition)
        {
            switch(definition->getDefinitionType())
            {
                case Definition::CARRY:
                    return CARRY;
                case Definition::INTERRUPT:
                    return INTERRUPT;
                case Definition::DECIMAL:
                    return DECIMAL;
                case Definition::OVERFLOW:
                    return OVERFLOW;
                case Definition::ZERO:
                    return ZERO;
                case Definition::NEGATIVE:
                    return NEGATIVE;
            }
        }
        return INVALID;
    }
    
    Argument::ArgumentType Argument::resolveIndexedType(const std::string& index)
    {
        Definition* definition = SymbolTable::getBuiltins()->tryGet(index);
        
        if(definition)
        {
            switch(definition->getDefinitionType())
            {
                case Definition::X:
                    return INDEXED_BY_X;
                case Definition::Y:
                    return INDEXED_BY_Y;
            }
        }
        return INVALID;
    }


    Argument::Argument(ArgumentType argumentType, SourcePosition* sourcePosition)
        : Node(sourcePosition), argumentType(argumentType), expression(0), zeroPage(false)
    {
    }
    
    Argument::Argument(ArgumentType argumentType, Expression* expression, SourcePosition* sourcePosition)
        : Node(sourcePosition), argumentType(argumentType), expression(expression), zeroPage(false)
    {
    }
    
    Argument::~Argument()
    {
        delete expression;
    }
    
    void Argument::checkForZeroPage()
    {
        // Check if the expression uses defined symbols, and if possible can be used in zero-page addressing.
        // If the value is defined but unknown at this pass, assume it won't fit in zero page and don't error.
        if(expression && expression->fold(false, true))
        {
            zeroPage = expression->getFoldedValue() < 256;
        }
    }
    
    void Argument::write(RomBank* bank)
    {
        // Only write data if there is an expression.
        if(expression)
        {
            if(expression->fold(true, true))
            {
                if(zeroPage)
                {
                    bank->writeByte(expression->getFoldedValue(), getSourcePosition());
                }
                else
                {
                    bank->writeWord(expression->getFoldedValue(), getSourcePosition());
                }
                
            }
            else
            {
                error("argument has indeterminate value", getSourcePosition());
            }
        }
    }
    
    void Argument::writeRelativeByte(RomBank* bank)
    {
        // Only write data if there is an expression.
        if(expression)
        {
            if(expression->fold(true, true))
            {
                // offset is amount to add to the PC to get the label position.
                int offset = (int) expression->getFoldedValue() - ((int) bank->getProgramCounter() + 1);
                unsigned char ofs = (unsigned char) offset;
                
                if(offset >= -128 && offset <= 127)
                {
                    bank->writeByte(ofs, getSourcePosition());
                }
                else
                {
                    std::ostringstream os;
                    os << "relative jump is outside of range -127..128 bytes. rewrite the branch or shorten the gaps in your code.";
                    os << "(pc = " << bank->getProgramCounter() << ", label = " << expression->getFoldedValue() <<
                        ", pc - label = " << offset << ")";
                    error(os.str(), getSourcePosition());
                }
                std::cout << "PC = ";
                std::cout << std::setfill('0') << std::setw(4) << std::hex << (int) bank->getProgramCounter();
                std::cout << ": ";
                std::cout << std::setfill('0') << std::setw(2) << std::hex << (int) ofs;
                std::cout << std::dec << " (" << offset << ")" << std::endl;
            }
            else
            {
                error("argument has indeterminate value", getSourcePosition());
            }
        }
    }
}