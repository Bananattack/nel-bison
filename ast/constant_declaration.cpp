#include "symbol_table.h"
#include "constant_declaration.h"
#include "constant_definition.h"

namespace nel
{
    ConstantDeclaration::ConstantDeclaration(StringNode* name, Expression* expression, SourcePosition* sourcePosition)
        : Statement(Statement::CONSTANT_DECLARATION, sourcePosition), name(name), expression(expression)
    {
    }

    ConstantDeclaration::~ConstantDeclaration()
    {
        delete name;
        delete expression;
    }
    
    void ConstantDeclaration::aggregate()
    {
        SymbolTable::getActiveScope()->put(new ConstantDefinition(name->getValue(), this), name->getSourcePosition());
    }

    void ConstantDeclaration::validate()
    {
    }
    
    void ConstantDeclaration::generate()
    {
    }
}