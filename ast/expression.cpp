#include <sstream>

#include "error.h"
#include "symbol_table.h"
#include "constant_definition.h"
#include "label_definition.h"
#include "variable_definition.h"
#include "expression.h"

namespace nel
{
    Expression::Expression(NumberNode* number, SourcePosition* sourcePosition)
        : Node(sourcePosition), expressionType(NUMBER), number(number)
    {
        init();
    }
    
    Expression::Expression(StringNode* identifier, SourcePosition* sourcePosition)
        : Node(sourcePosition), expressionType(IDENTIFIER), identifier(identifier)
    {
        init();
    }
    
    Expression::Expression(Operation* operation, SourcePosition* sourcePosition)
        : Node(sourcePosition), expressionType(OPERATION), operation(operation)
    {
        init();
    }
    
    Expression::~Expression()
    {
        switch(expressionType)
        {
            case NUMBER:
                delete number;
                break;
            case IDENTIFIER:
                delete identifier;
                break;
            case OPERATION:
                delete operation;
                break;
        }
    }
    
    void Expression::init()
    {
        folded = false;
        // An arbitrary uninitialized value outside of 0..65536,
        // to make use of unfolded expressions easier to debug.
        foldedValue = 0xDEADFACE;
    }
    
    bool Expression::fold(bool mustFold, bool forbidUndefined, std::vector<Definition*>& expansionStack)
    {
        // symbolStack contains a stack of all named constants that are being expanded.
        // This is the max size that this stack is allowed to grow.
        const unsigned int EXPANSION_STACK_MAX = 16;
        
        if(folded)
        {
            return true;
        }
        
        switch(expressionType)
        {
            case NUMBER:
            {
                folded = true;
                foldedValue = number->getValue();
                break;
            }
            case IDENTIFIER:
            {
                Definition* def = SymbolTable::getActiveScope()->tryGet(identifier->getValue());
                if(def)
                {
                    switch(def->getDefinitionType())
                    {
                        case Definition::CONSTANT:
                        {
                            ConstantDefinition* constant = (ConstantDefinition*) def;
                            Expression* expression = constant->getConstantDeclaration()->getExpression();
                            
                            expansionStack.push_back(constant);
                            if(expansionStack.size() > EXPANSION_STACK_MAX)
                            {
                                std::ostringstream os;
                                os << "too many constant expansions required (exceeded max depth " <<
                                    EXPANSION_STACK_MAX << "). are there mutually-dependent constants?";
                                for(size_t i = 0; i < expansionStack.size(); i++)
                                {
                                    Definition* entry = expansionStack[i];
                                    os << std::endl << "    `" << entry->getName() << "` at " <<
                                        entry->getDeclarationPoint();
                                }
                                error(os.str(), getSourcePosition(), true);
                            }
                            else
                            {
                                folded = expression->fold(mustFold, forbidUndefined, expansionStack);
                            }
                            expansionStack.pop_back();
                            
                            foldedValue = expression->getFoldedValue();
                            break;
                        }
                        case Definition::LABEL:
                        {
                            LabelDefinition* label = (LabelDefinition*) def;
                            folded = label->isLocationKnown();
                            foldedValue = label->getLocation();
                            break;
                        }
                        case Definition::VARIABLE:
                        {
                            VariableDefinition* var = (VariableDefinition*) def;
                            folded = true;
                            foldedValue = var->getOffset();
                            break;
                        }
                    }
                }
                
                if(!def && forbidUndefined)
                {
                    std::ostringstream os;
                    os << "identifier `" << identifier->getValue() << "` is not defined anywhere.";
                    error(os.str(), getSourcePosition());
                }
                else if(!folded && mustFold)
                {
                    std::ostringstream os;
                    os << "identifier `" << identifier->getValue() << "` has an indeterminate value.";
                    error(os.str(), getSourcePosition());
                }
                break;
            }
            case OPERATION:
            {
                operation->getLeft()->fold(mustFold, forbidUndefined, expansionStack);
                operation->getRight()->fold(mustFold, forbidUndefined, expansionStack);
                
                if(!operation->getLeft()->isFolded() || !operation->getRight()->isFolded())
                {
                    return false;
                }
                
                folded = true;
                
                unsigned int ls = operation->getLeft()->getFoldedValue();
                unsigned int rs = operation->getRight()->getFoldedValue();
                switch(operation->getOperationType())
                {
                    case Operation::MUL:
                    {
                        if(ls > MAX_VALUE / rs)
                        {
                            error("multiplication yields result which will overflow outside of 0..65535.", operation->getRight()->getSourcePosition());
                            folded = false;
                        }
                        else
                        {
                            foldedValue = ls * rs;
                        }
                        break;
                    }
                    case Operation::DIV:
                    {
                        if(rs == 0)
                        {
                            error("division by zero is undefined.", operation->getRight()->getSourcePosition());
                            folded = false;
                        }
                        else
                        {
                            foldedValue = ls / rs;
                        }
                        break;
                    }
                    case Operation::MOD:
                    {
                        if(rs == 0)
                        {
                            error("modulo by zero is undefined.", operation->getRight()->getSourcePosition());
                            folded = false;
                        }
                        else
                        {
                            foldedValue = ls / rs;
                        }
                        break;
                    }
                    case Operation::ADD:
                    {
                        if(ls + MAX_VALUE < rs)
                        {
                            error("addition yields result which will overflow outside of 0..65535.", operation->getRight()->getSourcePosition());
                            folded = false;
                        }
                        else
                        {
                            foldedValue = ls + rs;
                        }
                        break;
                    }
                    case Operation::SUB:
                    {
                        if(ls < rs)
                        {
                            error("subtraction yields result which will overflow outside of 0..65535.", operation->getRight()->getSourcePosition());
                            folded = false;
                        }
                        else
                        {
                            foldedValue = ls - rs;
                        }
                        break;
                    }
                    case Operation::SHL:
                    {
                        // If shifting more than N bits, or ls << rs > 2^N-1, then error.
                        if(rs > 16 || (rs > 0 && (ls & ~(1 << (16 - rs))) != 0))
                        {
                            error("logical shift left yields result which will overflow outside of 0..65535.", operation->getRight()->getSourcePosition());
                            folded = false;
                        }
                        else
                        {
                            foldedValue = ls << rs;
                        }
                        break;
                    }
                    case Operation::SHR:
                    {
                        foldedValue = ls >> rs;
                        break;
                    }
                    case Operation::BITWISE_AND:
                    {
                        foldedValue = ls & rs;
                        break;
                    }
                    case Operation::BITWISE_XOR:
                    {
                        foldedValue = ls ^ rs;
                        break;
                    }
                    case Operation::BITWISE_OR:
                    {
                        foldedValue = ls | rs;
                        break;
                    }
                }
                break;
            }
        }
        return folded;
    }
    
    bool Expression::fold(bool mustFold, bool forbidUndefined)
    {
        std::vector<Definition*> expansionStack;
        return fold(mustFold, forbidUndefined, expansionStack);
    }
}