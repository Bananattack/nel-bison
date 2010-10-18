#pragma once

#include "node.h"
#include "rom_bank.h"
#include "expression.h"

namespace nel
{
    /**
     * The argument to a kind of executable statement.
     * Used as the operand of a command or branching construct.
     */
    class Argument : public Node
    {
        public:
            /**
             * An enumeration of all the different kinds of arguments available.
             */
            enum ArgumentType
            {
                INVALID,                /**< A bad argument. */
                // Branch labels.
                LABEL,                  /**< A plain label. Used by goto/call. */
                INDIRECT_LABEL,         /**< An indirected label. Used by goto. */
                // Registers.
                A,                      /**< The accumulator A. */
                X,                      /**< The index register X. */
                Y,                      /**< The index register Y. */
                S,                      /**< The stack pointer S. */
                P,                      /**< The processor status P. */
                // P-Flags.
                CARRY,                  /**< The carry flag. set/unset/branch. */
                INTERRUPT,              /**< The interrupt disable flag. set/unset. */
                DECIMAL,                /**< The binary coded decimal mode flag. set/unset. */
                OVERFLOW,               /**< The overflow flag. unset/branch. */
                ZERO,                   /**< The zero flag. branch. */
                NEGATIVE,               /**< The negative flag. branch. */
                // Memory terms.
                IMMEDIATE,              /**< An immediate literal value. */
                DIRECT,                 /**< Direct memory addressing. */
                INDEXED_BY_X,           /**< Direct memory indexing by register X. */
                INDEXED_BY_Y,           /**< Direct memory indexing by register Y. */
                ZP_INDEXED_INDIRECT,    /**< Indirectly addresses a value retreived by indexing the zero page. */
                ZP_INDIRECT_INDEXED,    /**< Indexes a location that is indirectly addressesed by a value in the zero page. */
            };
            
            /**
             * Gets the argument type of an unprefixed term, which must be a register or p-flag.
             * Returns the argument type associated with the name if it is a built-in, and INVALID otherwise.
             */
            static ArgumentType resolveUnprefixedBuiltinType(const std::string& name);
            
            /**
             * Gets the argument type of a p-flag. Returns a p-flag argument type if valid, or INVALID if not a p-flag.
             */
            static ArgumentType resolveConditionType(const std::string& name);
            
            /**
             * Gets the ArgumentType of a register involved in a direct memory indexing operation.
             * Returns X, Y, or INVALID.
             */
            static ArgumentType resolveIndexedType(const std::string& index);
        private:
            ArgumentType argumentType;
            Expression* expression;
            bool zeroPage;
            
        public:
            Argument(ArgumentType argumentType, SourcePosition* sourcePosition);
            Argument(ArgumentType argumentType, Expression* expression, SourcePosition* sourcePosition);
            ~Argument();
            
            /**
             * Returns the kind of argument that this node represents.
             */
            ArgumentType getArgumentType()
            {
                return argumentType;
            }
            
            /**
             * Returns the expression associated with this argument, or 0 when it is not appropriate.
             */
            Expression* getExpression()
            {
                return expression;
            }
            
            /**
             * Returns whether an argument has an expression which is in the range 0..255.
             */
            bool isZeroPage()
            {
                return zeroPage;
            }
            
            /**
             * Force the argument to be zero-page.
             */
            void forceZeroPage()
            {
                zeroPage = true;
            }
            
            /**
             * Returns whether the expression type is a memory term (ie not a register or carry flag).
             */
            bool isMemoryTerm()
            {
                switch(argumentType)
                {
                    case IMMEDIATE:
                    case DIRECT:
                    case INDEXED_BY_X:
                    case INDEXED_BY_Y:
                    case ZP_INDEXED_INDIRECT:
                    case ZP_INDIRECT_INDEXED:
                        return true;
                    default:
                        return false;
                }
            }
            
            /**
             * Examines this term to figure out if it could fit in zero page.
             * After it's done, isZeroPage() will reflect the result.
             */
            void checkForZeroPage();
            
            /**
             * Writes the expression, using a single byte if zero page,
             * and two bytes otherwise. Writes nothing (no error) if no expression.
             */
            void write(RomBank* bank);
            
            /**
             * Writes the expression as relative offset from program counter.
             * This offset is a 2's complement number in the range -127..128.
             * It is an error if the expression is undefined or outside of this range.
             */
            void writeRelativeByte(RomBank* bank);
    };
    
}