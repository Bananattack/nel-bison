#pragma once

#include "definition.h"
#include "block_statement.h"

namespace nel
{
    /**
     * This represents a Definition that pertains to a package,
     * which is created upon entering a named package scope.
     */
    class PackageDefinition : public Definition
    {
        private:
            SymbolTable* scope;
            
        public:    
            PackageDefinition(std::string name, SymbolTable* scope)
                : Definition(Definition::PACKAGE, name), scope(scope)
            {
            }
            
            /**
             * Get the scope associated with this package definition.
             */
            SymbolTable* getScope()
            {
                return scope;
            }
    };
}