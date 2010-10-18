#pragma once

#include "definition.h"
#include "constant_declaration.h"

namespace nel
{
    /**
     * This represents a Definition that pertains to a constant,
     * which is created upon that constant's declaration.
     */
    class ConstantDefinition : public Definition
    {
        private:
            ConstantDeclaration* constantDeclaration;
            
        public:    
            ConstantDefinition(std::string name, ConstantDeclaration* constantDeclaration)
                : Definition(Definition::CONSTANT, name), constantDeclaration(constantDeclaration)
            {
            }
            
            /**
             * Get the declaration associated with this definition.
             */
            ConstantDeclaration* getConstantDeclaration()
            {
                return constantDeclaration;
            }
    };
}