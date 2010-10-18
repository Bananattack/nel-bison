#pragma once

#include "list_node.h"
#include "statement.h"
#include "argument.h"
#include "command.h"

namespace nel
{
    /**
     * A command statement, which has a receiver, and one or more commands that use that receiver.
     */
    class CommandStatement : public Statement
    {
        private:
            Argument* receiver;
            ListNode<Command*>* commands;
            
        public:    
            CommandStatement(Argument* receiver, ListNode<Command*>* commands, SourcePosition* sourcePosition);
            ~CommandStatement();
            
            /**
             * Returns the receiver of the commands in this statement.
             */
            Argument* getReceiver()
            {
                return receiver;
            }
            
            /**
             * Returns the list of commands in this statement.
             */
            ListNode<Command*>* getCommands()
            {
                return commands;
            }

            void aggregate();
            void validate();
            void generate();
    };
}