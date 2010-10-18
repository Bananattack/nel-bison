#pragma once

#include "node.h"
#include "rom_bank.h"
#include "argument.h"

namespace nel
{
    /**
     * An individual command belonging to a collective command statement.
     * These comprise of most executable statements in the language which
     * don't affect program flow.
     */
    class Command : public Node
    {
        public:
            /**
             * An enumeration of all the different kinds of commands
             * that can belong to a command statement.
             */
            enum CommandType
            {
                INVALID,        /**< An invalid command. */
                // These have a receiver and require some sort of immediate/memory argument (error if there is none)
                GET,            /**< get command. Transfer a value from the argument into the receiver. */
                PUT,            /**< put command. Transfers a value from the receiver into the argument. */
                ADD,            /**< add command. Synthetic instruction to add without carry. */
                ADDC,           /**< addc command. Adds with carry. */
                SUB,            /**< subtract command. Synthetic instruction to subtract without carry. */
                SUBC,           /**< subc command. Subtract with carry (1 - borrow) command. */
                BITWISE_OR,     /**< or command. Bitwise OR between receiver and argument. */
                BITWISE_AND,    /**< and command. Bitwise AND between receiver and argument. */
                BITWISE_XOR,    /**< xor command. Bitwise XOR between receiver and argument. */
                CMP,    /**< cmp command. Compares the argument to the receiver. */
                BIT,    /**< bit command. Does a weird bitwise test between receiver and argument. */
                // These have a receiver but no argument
                INC,    /**< inc command. Increments receiver. */
                DEC,    /**< dec command. Decrements receiver. */
                NOT,    /**< not command. Synthetic instruction to bitwise NOT the receiver. */
                NEG,    /**< neg command. Synthetic instruction to do arithmetic 2's complement negation. */
                SHL,    /**< shl command. Logically shifts the receiver left by one bit. */
                SHR,    /**< shl command. Logically shifts the receiver right by one bit. */
                ROL,    /**< rol command. Rotates the receiver left by one bit through carry. */
                ROR,    /**< ror command. Rotates the receiver right by one bit through carry. */
                PUSH,   /**< push command. Pushes receiver onto stack. */
                PULL,   /**< pull command. Pulls receiver from stack. */
                // These have P as a receiver, and require a p-flag argument (error if there is none)
                SET,    /**< set command. Turns on a p-flag. */
                UNSET   /**< unset command. Turns on a p-flag. */
            };

            /**
             * Returns a string representation of this command's type.
             */
            static std::string getCommandName(CommandType commandType)
            {
                switch(commandType)
                {
                    case GET:           return "get";
                    case PUT:           return "put";
                    case ADD:           return "add";
                    case ADDC:          return "addc";
                    case SUB:           return "sub";
                    case SUBC:          return "subc";
                    case BITWISE_OR:    return "or";
                    case BITWISE_AND:   return "and";
                    case BITWISE_XOR:   return "xor";
                    case CMP:           return "cmp";
                    case BIT:           return "bit";
                    case INC:           return "inc";
                    case DEC:           return "dec";
                    case NOT:           return "not";
                    case NEG:           return "neg";
                    case SHL:           return "shl";
                    case SHR:           return "shr";
                    case ROL:           return "rol";
                    case ROR:           return "ror";
                    case PUSH:          return "push";
                    case PULL:          return "pull";
                    case SET:           return "set";
                    case UNSET:         return "unset";
                    default:            return "";
                }
            }
            
        private:
            CommandType commandType;
            // The old command type that this used to be before some transformation.
            CommandType oldCommandType;
            // The receiver of this command. Filled in by a DirectedCommandStatement.
            Argument* receiver;
            // The argument to pass this command, might be null. 
            Argument* argument;
            
        public:
            Command(CommandType commandType, SourcePosition* sourcePosition);
            Command(CommandType commandType, Argument* argument, SourcePosition* sourcePosition);
            ~Command();
            
        private:
            void init();
            void commandError(std::string msg);
            unsigned int calculateGetSize(Argument* receiver, Argument* argument);
            unsigned int calculatePutSize(Argument* receiver, Argument* argument);
            
        public:
            /**
             * Returns the type of command this node represents.
             */
            CommandType getCommandType()
            {
                return commandType;
            }
            
            /**
             * Returns the receiver of this command, as set by a prior setReceiver() call.
             */            
            Argument* getReceiver()
            {
                return receiver;
            }
            
            /**
             * Sets the receiver argument associated with this command.
             * Does not claim "ownership" of this pointer on assignment, so keep it around.
             */
            void setReceiver(Argument* value)
            {
                this->receiver = value;
            }
            
            /**
             * Returns the argument of this command.
             */
            Argument* getArgument()
            {
                return argument;
            }
            
            /**
             * Returns the size of the full instruction in bytes, or 0 if invalid.
             */
            unsigned int calculateSize();
            
            /**
             * Writes this command into the rom.
             */
            void write(RomBank* bank);
    };
}