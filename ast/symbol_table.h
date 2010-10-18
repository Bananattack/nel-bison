#include <vector>

#include "map.h"
#include "definition.h"

namespace nel
{
    /**
     * A table which maps various symbols in code to definitions.
     * This is used for most labels, variables, constants and builtins.
     */
	class SymbolTable
	{
        private:
            typedef Map<std::string, Definition*>::Type Dictionary;
			typedef Dictionary::iterator DictIterator;
        
            // Scope used for constants, variables and label declarations.
            // The current top of the scopeStack.
            static SymbolTable* activeScope;
            // A stack of all depths of active scopes.
            // Necessary for checking when a label is declared outside of the current scope
            // (but contained by an outer scope the inner scope can reference).
            static std::vector<SymbolTable*> scopeStack;
            // Contains the builtins.
            static SymbolTable* builtins;
            
		public:
            /**
             * Get the symbol table containing the built-in definitions.
             */
            static SymbolTable* getBuiltins();
            
            /**
             * Get the active scope containing user-supplied definitions.
             */
            static SymbolTable* getActiveScope();
            
            /**
             * Enters a new scope level, pushing it onto the scope stack.
             */
            static void enterScope(SymbolTable* symbolTable);
            
            /**
             * Exits the current scope level, returning to its parent scope.
             */
            static void exitScope();
        
        private:
			// The outer scope containing this scope. NULL if it does not apply.
			SymbolTable* parent;
			// The symbol table
			Dictionary dict;
            // Depth of scope relative to other scopes.
            int depth;
            
		public:
			SymbolTable(SymbolTable* parent = 0);
			~SymbolTable();

			/**
             * Insert symbol into the local scope.
             */
			void put(Definition* value, SourcePosition* sourcePosition);
            
			/**
             * Perform search with inheritance setting to see if this
             * binding exists at this or an earlier scope.
             */
			Definition* get(std::string name, SourcePosition* sourcePosition);
            
			/**
             * Attempts to get a symbol (using inheritance setting provided).
             * Returns NULL on failure.
             */
            Definition* tryGet(std::string key, bool useInheritance = true);
	};
}