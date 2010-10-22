#pragma once

#include "source_position.h"

namespace nel
{
    /**
     * A definition of some piece of the language, which can
     * be looked up in a symbol table. For example, a variable,
     * constant, label or built-in register.
     */
    class Definition
    {
        public:
            /**
             * An enumeration of all valid definitions.
             */
            enum DefinitionType
            {
                INVALID,    /**< An invalid or unknown definition. */
                // Built-in definitions.
                A,          /**< The accumulator. */
                X,          /**< The index register X. */
                Y,          /**< The index register Y. */
                S,          /**< The stack pointer */
                P,          /**< The processor status */
                CARRY,      /** Carry flag. */
                INTERRUPT,  /**< Interrupt disable flag */
                DECIMAL,    /**< Binary coded decimal mode flag */
                OVERFLOW,   /**< Overflow flag. */
                ZERO,       /**< Zero flag. */
                NEGATIVE,   /**< Negative flag. */
                // User definitions.
                VARIABLE,   /**< A variable in RAM. */
                CONSTANT,   /**< A named constant. */
                LABEL,      /**< A label for code/data. */
                PACKAGE,    /**< A named scope / namespace. */
            };
            
        private:
            DefinitionType definitionType;
            SourcePosition* declarationPoint;
            std::string name;
            
        public:
            Definition(DefinitionType definitionType, std::string name)
                : definitionType(definitionType), declarationPoint(0), name(name)
            {
            }
            
            virtual ~Definition()
            {
                delete declarationPoint;
            }
            
            /**
             * Returns the type of definition this is.
             */
            DefinitionType getDefinitionType()
            {
                return definitionType;
            }

            /**
             * Returns the point at which this symbol was defined,
             * and 0 if it's otherwise built-in or unknown.
             */            
            SourcePosition* getDeclarationPoint()
            {
                return declarationPoint;
            }
            
            /**
             * Sets the point at which this symbol was defined.
             */            
            void setDeclarationPoint(SourcePosition* value)
            {
                declarationPoint = value;
            }
            
            /**
             * Returns the name associated with this definition.
             */
            std::string getName()
            {
                return name;
            }
    };
}