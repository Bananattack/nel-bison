#include "error.h"
#include "rom_generator.h"
#include "rom_bank.h"
#include "branch_statement.h"

namespace nel
{
    BranchStatement::BranchStatement(BranchType branchType, SourcePosition* sourcePosition)
        : Statement(Statement::BRANCH, sourcePosition), branchType(branchType), destination(0), condition(0)
    {
    }

    BranchStatement::BranchStatement(BranchType branchType, Argument* destination, SourcePosition* sourcePosition)
        : Statement(Statement::BRANCH, sourcePosition), branchType(branchType), destination(destination), condition(0)
    {
    }
    
    BranchStatement::BranchStatement(BranchType branchType, Argument* destination, BranchCondition* condition, SourcePosition* sourcePosition)
        : Statement(Statement::BRANCH, sourcePosition), branchType(branchType), destination(destination), condition(condition)
    {
    }

    BranchStatement::~BranchStatement()
    {
        delete destination;
        delete condition;
    }
    
    void BranchStatement::aggregate()
    {
    }

    void BranchStatement::validate()
    {
        unsigned int size = 0;
        switch(branchType)
        {
            case NOP:
            case RETURN:
            case RTI:
                // Implicit operand.
                size = 1;
                break;
            case GOTO:
                // Conditional goto has relative offset -128..127, regular goto is absolute 16-bit address.
                size = condition ? 2 : 3;
                break;
            case CALL:
                // Absolute 16-bit location.
                size = 3;
                break;
        }
        
        // Reserve the bytes needed for this data.
        RomBank* bank = romGenerator->getActiveBank();
        if(!bank)
        {
            error("branch statement found, but a rom bank hasn't been selected yet", getSourcePosition(), true);
            return;
        }
        bank->expand(size, getSourcePosition());
    }
    
    void BranchStatement::generate()
    {
        // Get the bank to use for writing.
        RomBank* bank = romGenerator->getActiveBank();
        if(!bank)
        {
            error("branch statement found, but a rom bank hasn't been selected yet", getSourcePosition(), true);
            return;
        }
        
        switch(branchType)
        {
            case NOP:
                bank->writeByte(0xEA, getSourcePosition()); // nop
                break;
            case RETURN:
                bank->writeByte(0x60, getSourcePosition()); // rts
                break;
            case RTI:
                bank->writeByte(0x40, getSourcePosition()); // rti
                break;
            case GOTO:
                if(condition)
                {
                    unsigned int opcode = 0;
                    if(destination->getArgumentType() == Argument::INDIRECT_LABEL)
                    {
                        error("goto [indirect] cannot have a `when` clause.", getSourcePosition());
                    }
                    switch(condition->getFlag()->getArgumentType())
                    {
                        case Argument::CARRY:
                            // bcs / bcc
                            opcode = condition->getConditionType() == BranchCondition::CONDITION_SET ? 0xB0 : 0x90;
                            break;
                        case Argument::ZERO:
                            // beq / bne
                            opcode = condition->getConditionType() == BranchCondition::CONDITION_SET ? 0xF0 : 0xD0;
                            break;
                        case Argument::NEGATIVE:
                            // bmi / bpl
                            opcode = condition->getConditionType() == BranchCondition::CONDITION_SET ? 0x30 : 0x10;
                            break;                        
                        case Argument::OVERFLOW:
                            // bvs / bvc
                            opcode = condition->getConditionType() == BranchCondition::CONDITION_SET ? 0x70 : 0x50;
                            break;
                        default:
                            error("goto condition provided must be `carry`, `zero`, `negative`, or `overflow`", getSourcePosition());
                    }
                    bank->writeByte(opcode, getSourcePosition());
                    destination->writeRelativeByte(bank);
                }
                else
                {
                    unsigned int opcode = 0;
                    switch(destination->getArgumentType())
                    {
                        case Argument::LABEL:
                            opcode = 0x4C; // jmp label
                            break;
                        case Argument::INDIRECT_LABEL:
                            opcode = 0x6C; // jmp [indirect]
                            break;
                    }
                    
                    bank->writeByte(opcode, getSourcePosition());
                    destination->write(bank);
                }
                break;
            case CALL:
                if(destination->getArgumentType() == Argument::INDIRECT_LABEL)
                {
                    error("`call` cannot take an [indirect] memory location.", getSourcePosition());
                }
                bank->writeByte(0x20, getSourcePosition()); // jsr label
                destination->write(bank);
                break;
        }
    }
}