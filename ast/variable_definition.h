#pragma once

#include <iostream>

#include "definition.h"
#include "variable_declaration.h"

namespace nel
{
    /**
     * This represents a Definition that pertains to a variable,
     * which is created upon that variable's declaration.
     */
    class VariableDefinition : public Definition
    {
        private:
            VariableDeclaration* variableDeclaration;
            unsigned int offset;
            
        public:    
            VariableDefinition(std::string name, VariableDeclaration* variableDeclaration, unsigned int offset)
                : Definition(Definition::VARIABLE, name), variableDeclaration(variableDeclaration), offset(offset)
            {
            }
            
            /**
             * Get the declaration associated with this definition.
             */
            VariableDeclaration* getVariableDeclaration()
            {
                return variableDeclaration;
            }
            
            /**
             * Returns the RAM address of this particular variable, which
             * may appear in a compound variable declaration.
             */
            unsigned int getOffset()
            {
                return offset;
            }
    };
}