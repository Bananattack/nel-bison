#pragma once

#include "statement.h"
#include "string_node.h"
#include "list_node.h"

namespace nel
{
    class SymbolTable;
    
    /**
     * A compound block statement, used for scoping.
     * Scopes can be defined by the begin/end constructs in nel.
     * The main program chunk is implicitly enclosed in a block.
     */
    class BlockStatement : public Statement
    {
        public:
            /**
             * An enumeration of all the different kinds of blocks within the language.
             */
            enum BlockType
            {
                MAIN,   /**< The implicit block enclosing the program. */
                SCOPE   /**< An explicitly defined scope. */
            };
            
        private:
            BlockType blockType;
            StringNode* name;
            ListNode<Statement*>* statements;
            SymbolTable* scope;
            
        public:    
            BlockStatement(BlockType blockType, ListNode<Statement*>* statements, SourcePosition* sourcePosition);
            BlockStatement(BlockType blockType, StringNode* name, ListNode<Statement*>* statements, SourcePosition* sourcePosition);
            ~BlockStatement();
            
        private:
            bool handleHeader(ListNode<Statement*>::ListType& list);

        public:
            /**
             * Returns the type of block this is.
             */
            BlockType getBlockType()
            {
                return blockType;
            }

            /**
             * Returns the list of statements contained in this block.
             */            
            ListNode<Statement*>* getStatements()
            {
                return statements;
            }
            
            /**
             * Returns the symbol table associated with this scope.
             */
            SymbolTable* getScope()
            {
                return scope;
            }

            /**
             * Returns the StringNode representing name of this
             * scope (for packages), or 0 if there is none.
             */
            StringNode* getName()
            {
                return name;
            }

            void aggregate();
            void validate();
            void generate();
    };
}
