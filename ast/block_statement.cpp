#include <sstream>

#include "error.h"
#include "rom_generator.h"
#include "block_statement.h"
#include "header_statement.h"
#include "symbol_table.h"
#include "package_definition.h"

namespace nel
{
    BlockStatement::BlockStatement(BlockType blockType, ListNode<Statement*>* statements, SourcePosition* sourcePosition)
        : Statement(Statement::BLOCK, sourcePosition), blockType(blockType), name(0), statements(statements), scope(0)
    {
    }

    BlockStatement::BlockStatement(BlockType blockType, StringNode* name, ListNode<Statement*>* statements, SourcePosition* sourcePosition)
        : Statement(Statement::BLOCK, sourcePosition), blockType(blockType), name(name), statements(statements), scope(0)
    {
    }
    
    BlockStatement::~BlockStatement()
    {
        delete name;
        delete statements;
        delete scope;
    }
    
    // Find and handle the header for the main block.
    bool BlockStatement::handleHeader(ListNode<Statement*>::ListType& list)
    {
        Statement* header = 0;
        for(size_t i = 0; i < list.size(); i++)
        {
            Statement* statement = list[i];
            if(statement->getStatementType() == Statement::HEADER)
            {
                if(blockType == MAIN)
                {
                    if(!header)
                    {
                        header = list[i];
                    }
                    else
                    {
                        std::ostringstream os;
                        os << "multiple ines headers found. (previous header at ";
                        header->getSourcePosition()->print(os);
                        os << ").";
                        error(os.str(), statement->getSourcePosition(), true);
                        return false;
                    }
                }
                else
                {
                    std::ostringstream os;
                    os << "ines header cannot appear inside a begin/end block.";
                    error(os.str(), statement->getSourcePosition(), true);
                    return false;       
                }
            }
            else if(blockType == MAIN && !header)
            {
                if(statement->getStatementType() != Statement::CONSTANT_DECLARATION)
                {
                    error("statement that is not a constant declaration found before the ines header.", statement->getSourcePosition(), true);
                    return false;
                }
            }
        }
        
        if(blockType == MAIN)
        {
            if(!header)
            {
                error("no ines header found.", getSourcePosition(), true);
                return false;
            }
            
            header->aggregate();
        }
        return true;
    }

    void BlockStatement::aggregate()
    {
        // Create scope.
        scope = new SymbolTable(SymbolTable::getActiveScope());

        // If this scope has a name, register that as
        // a member of the containing scope.
        if(name)
        {
            // Assumes this code will never get executed by the outermost block (before there is an active scope).
            SymbolTable* outer = SymbolTable::getActiveScope();

            PackageDefinition* package = new PackageDefinition(name->getValue(), scope);

            // Shove the this package into the containing outer scope.
            outer->put(package, getSourcePosition());
            // Create a back-reference, so the scope knows it defines a new package,
            // rather than just sharing a private subset of the outer scope's package.
            scope->setPackage(package);
        }

        // Enter the created scope.
        SymbolTable::enterScope(scope);
        
        ListNode<Statement*>::ListType& list = statements->getList();
        // First gather all constant definitions in this scope.
        for(size_t i = 0; i < list.size(); i++)
        {
            Statement* statement = list[i];
            if(statement->getStatementType() == Statement::CONSTANT_DECLARATION)
            {
                statement->aggregate();
            }
        }
        
        // Next, if this is the main block, calculate the header.
        if(handleHeader(list))
        {
            // Now, check out all the other statements.
            for(size_t i = 0; i < list.size(); i++)
            {
                Statement* statement = list[i];
                switch(statement->getStatementType())
                {
                    // Skip already handled cases.
                    case Statement::CONSTANT_DECLARATION:
                    case Statement::HEADER:
                        break;
                    // Aggregate the rest.
                    default:
                        statement->aggregate();
                        break;
                }
            }
        }
        
        SymbolTable::exitScope();
    }
    
    void BlockStatement::validate()
    {
        SymbolTable::enterScope(scope);
        
        ListNode<Statement*>::ListType& list = statements->getList();
        // Check out all the statements that this contains.
        for(size_t i = 0; i < list.size(); i++)
        {
            list[i]->validate();
        }
        
        SymbolTable::exitScope();
    }
    
    void BlockStatement::generate()
    {
        SymbolTable::enterScope(scope);
        
        ListNode<Statement*>::ListType& list = statements->getList();
        // Check out all the statements that this contains.
        for(size_t i = 0; i < list.size(); i++)
        {
            list[i]->generate();
        }
        
        SymbolTable::exitScope();
    }
}