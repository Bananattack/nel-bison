#include <sstream>

#include "error.h"
#include "symbol_table.h"

namespace nel
{
    SymbolTable* SymbolTable::activeScope = 0;
    std::vector<SymbolTable*> SymbolTable::scopeStack;
    SymbolTable* SymbolTable::builtins = 0;
    
    SymbolTable* SymbolTable::getBuiltins()
    {
        // First time? init the builtins table.
        if(!builtins)
        {
            builtins = new SymbolTable();
            builtins->dict["a"] = new Definition(Definition::A, "a");
            builtins->dict["x"] = new Definition(Definition::X, "x");
            builtins->dict["y"] = new Definition(Definition::Y, "y");
            builtins->dict["s"] = new Definition(Definition::S, "s");
            builtins->dict["p"] = new Definition(Definition::P, "p");
            builtins->dict["carry"] = new Definition(Definition::CARRY, "carry");
            builtins->dict["interrupt"] = new Definition(Definition::INTERRUPT, "interrupt");
            builtins->dict["decimal"] = new Definition(Definition::DECIMAL, "decimal");
            builtins->dict["overflow"] = new Definition(Definition::OVERFLOW, "overflow");
            builtins->dict["zero"] = new Definition(Definition::ZERO, "zero");
            builtins->dict["negative"] = new Definition(Definition::NEGATIVE, "negative");
        }
        return builtins;
    }
    
    SymbolTable* SymbolTable::getActiveScope()
    {
        return activeScope;
    }
    
    void SymbolTable::enterScope(SymbolTable* symbolTable)
    {
        scopeStack.push_back(symbolTable);
        activeScope = symbolTable;
    }
    
    void SymbolTable::exitScope()
    {
        scopeStack.pop_back();

        if(scopeStack.size() > 0)
        {
            activeScope = scopeStack[scopeStack.size() - 1];
        }
        else
        {
            activeScope = 0;
        }
    }
    
    SymbolTable::SymbolTable(SymbolTable* parent)
        : parent(parent)
    {
        if(parent)
        {
            depth = parent->depth + 1;
        }
        else
        {
            depth = 0;
        }
    }
    
    SymbolTable::~SymbolTable()
    {
        DictIterator it;
        for(it = dict.begin(); it != dict.end(); ++it)
        {
            delete it->second;
        }
        dict.clear();
    }
    
    void SymbolTable::put(Definition* def, SourcePosition* sourcePosition)
    {
        // Perform search without inheritance to only whine if the symbol was already declared in this scope.
        // This way functions can have locals that use the same name as somewhere in the parent, without problems.
        // (get always looks at current scope and works up, there is no ambiguity)
        Definition* match = tryGet(def->getName(), false);
        
        if(match)
        {
            SourcePosition* previousDecl = match->getDeclarationPoint();
            
            std::ostringstream message;            
            message << "redefinition of symbol `" << def->getName() << "`, previously defined at ";
            if(previousDecl)
            {
                previousDecl->print(message);
            }
            else
            {
                message << "???";
            }
            error(message.str(), sourcePosition);
        }
        
        def->setDeclarationPoint(new SourcePosition(sourcePosition));
        dict[def->getName()] = def;
    }
    
    Definition* SymbolTable::get(std::string name, SourcePosition* sourcePosition)
    {
        Definition* def = tryGet(name, true);
        
        if(!def)
        {
            std::ostringstream message;
            message << "reference to undefined symbol `" << name << "`";
            error(message.str(), sourcePosition);
            return 0;
        }
        return def;
    }
    
    Definition* SymbolTable::tryGet(std::string name, bool useInheritance)
    {
        DictIterator it = dict.find(name);
        
        if(it == dict.end())
        {
            if(useInheritance && parent)
            {
                return parent->tryGet(name, useInheritance);
            }
            return 0;
        }
        return it->second;
    }
}