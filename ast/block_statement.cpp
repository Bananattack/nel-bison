#include <sstream>

#include "error.h"
#include "rom_generator.h"
#include "block_statement.h"
#include "header_statement.h"
#include "symbol_table.h"

namespace nel
{
    BlockStatement::BlockStatement(BlockType blockType, ListNode<Statement*>* statements, SourcePosition* sourcePosition)
        : Statement(Statement::BLOCK, sourcePosition), blockType(blockType), statements(statements), scope(0)
    {
    }
    
    BlockStatement::~BlockStatement()
    {
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
        scope = new SymbolTable(SymbolTable::getActiveScope());
        SymbolTable::enterScope(scope);
        
        ListNode<Statement*>::ListType list = statements->getList();
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
        
        ListNode<Statement*>::ListType list = statements->getList();
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
        
        ListNode<Statement*>::ListType list = statements->getList();
        // Check out all the statements that this contains.
        for(size_t i = 0; i < list.size(); i++)
        {
            list[i]->generate();
        }
        
        SymbolTable::exitScope();
    }
}