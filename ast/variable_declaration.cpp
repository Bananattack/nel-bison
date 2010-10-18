#include "error.h"
#include "symbol_table.h"
#include "variable_declaration.h"
#include "variable_definition.h"
#include "rom_generator.h"

namespace nel
{
    VariableDeclaration::VariableDeclaration(VariableType variableType, ListNode<StringNode*>* names, SourcePosition* sourcePosition)
        : Statement(Statement::VARAIBLE_DECLARATION, sourcePosition), variableType(variableType), names(names), arraySizeExpression(0)
    {
    }

    VariableDeclaration::VariableDeclaration(VariableType variableType, ListNode<StringNode*>* names, Expression* arraySizeExpression, SourcePosition* sourcePosition)
        : Statement(Statement::VARAIBLE_DECLARATION, sourcePosition), variableType(variableType), names(names), arraySizeExpression(arraySizeExpression)
    {
    }

    VariableDeclaration::~VariableDeclaration()
    {
        delete names;
        delete arraySizeExpression;
    }

    void VariableDeclaration::aggregate()
    {
        unsigned int size = variableType == WORD ? 2 : 1;
        
        if(arraySizeExpression)
        {
            if(!arraySizeExpression->fold(true, false))
            {
                error("could not resolve the array size provided to this variable declaration", getSourcePosition());
                return;
            }
            else
            {
                if(arraySizeExpression->getFoldedValue() == 0)
                {
                    error(
                        "an array size of 0 is invalid. why ask for a variable that can hold nothing?",
                        getSourcePosition()
                    );
                    
                    return;
                }
                else
                {
                    size *= arraySizeExpression->getFoldedValue();
                }
            }
        }
        
        
        ListNode<StringNode*>::ListType& list = names->getList();
        
        if(!romGenerator->isRamCounterSet())
        {
            error("variable declaration was found before the ram location was set.", getSourcePosition(), true);
        }
        
        for(size_t i = 0; i < list.size(); i++)
        {
            StringNode* name = list[i];
            // Insert symbol, using current RAM counter value as var offset.
            SymbolTable::getActiveScope()->put(new VariableDefinition(
                    name->getValue(),
                    this,
                    romGenerator->getRamCounter()
                ), name->getSourcePosition()
            );
            
            // Reserve size bytes in RAM counter to advance it forward.
            romGenerator->expandRam(size, getSourcePosition());
        }
    }

    void VariableDeclaration::validate()
    {
    }
    
    void VariableDeclaration::generate()
    {
    }
}