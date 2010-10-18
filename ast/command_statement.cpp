#include "error.h"
#include "rom_generator.h"
#include "rom_bank.h"
#include "command_statement.h"

namespace nel
{
    CommandStatement::CommandStatement(Argument* receiver, ListNode<Command*>* commands, SourcePosition* sourcePosition)
        : Statement(Statement::COMMAND, sourcePosition), receiver(receiver), commands(commands)
    {
        ListNode<Command*>::ListType& list = commands->getList();
        for(size_t i = 0; i < list.size(); i++)
        {
            Command* command = list[i];
            if(command)
            {
                command->setReceiver(receiver);
            }
        }
    }

    CommandStatement::~CommandStatement()
    {
        delete receiver;
        delete commands;
    }
    
    void CommandStatement::aggregate()
    {        
    }

    void CommandStatement::validate()
    {
        // Reserve the bytes needed for this data.
        RomBank* bank = romGenerator->getActiveBank();
        if(!bank)
        {
            error("command statement found, but a rom bank hasn't been selected yet", getSourcePosition(), true);
            return;
        }
        
        ListNode<Command*>::ListType& list = commands->getList();
        for(size_t i = 0; i < list.size(); i++)
        {
            Command* command = list[i];
            if(command)
            {
                unsigned int size = command->calculateSize();
                bank->expand(size, command->getSourcePosition());
            }
        }
    }
    
    void CommandStatement::generate()
    {
        // Get the bank to use for writing.
        RomBank* bank = romGenerator->getActiveBank();
        if(!bank)
        {
            error("command statement found, but a rom bank hasn't been selected yet", getSourcePosition(), true);
            return;
        }
        
        ListNode<Command*>::ListType& list = commands->getList();
        for(size_t i = 0; i < list.size(); i++)
        {
            Command* command = list[i];
            if(command)
            {
                command->write(bank);
            }
        }
    }
}