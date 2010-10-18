#include <sstream>

#include "error.h"
#include "command.h"

namespace nel
{
    Command::Command(CommandType commandType, SourcePosition* sourcePosition)
        : Node(sourcePosition), commandType(commandType), argument(0)
    {
        init();
    }

    Command::Command(CommandType commandType, Argument* argument, SourcePosition* sourcePosition)
        : Node(sourcePosition), commandType(commandType), argument(argument)
    {
        init();
    }
    
    Command::~Command()
    {
        delete argument;
    }
 
    void Command::init()
    {
        oldCommandType = INVALID;
        receiver = 0;
    }
    
    void Command::commandError(std::string msg)
    {
        std::ostringstream os;
        os << "invalid `" << getCommandName(commandType) << "` command";
        if(oldCommandType != INVALID)
        {
            os << " (converted from `M: " << getCommandName(oldCommandType) << " argument`)";
        }
        os << ": " << msg;
        error(os.str(), getSourcePosition());
    }
    
    unsigned int Command::calculateSize()
    {
        // Not possible by any command, and this prevents an infinite recursion
        // in some statements like get which convert 'M: get src' into 'src: put M'.
        if(receiver && receiver->isMemoryTerm() && argument && argument->isMemoryTerm())
        {
            commandError("receiver and argument cannot both be memory terms.");
            return 0;
        }
        
        unsigned int size = 0;

        switch(commandType)
        {
            case GET:
                switch(receiver->getArgumentType())
                {
                    case Argument::A:
                        switch(argument->getArgumentType())
                        {
                            case Argument::X:
                            case Argument::Y:
                                size = 1;
                                break;
                            case Argument::IMMEDIATE:
                            case Argument::ZP_INDEXED_INDIRECT:
                            case Argument::ZP_INDIRECT_INDEXED:
                                argument->forceZeroPage();
                                size = 2;
                                break;
                            case Argument::DIRECT:
                            case Argument::INDEXED_BY_X:
                                argument->checkForZeroPage();
                                size = argument->isZeroPage() ? 2 : 3;
                                break;
                            case Argument::INDEXED_BY_Y:
                                size = 3;
                                break;
                            default:
                                commandError("if receiver is the register `a`, then the argument must be the register `x` or `y`, an immediate value #foo, a direct memory term of form @foo, @foo[x] or @foo[y], or an indirect term of form @[foo[x]] or @[foo][y]");
                                break;
                        }
                        break;
                    case Argument::X:
                        switch(argument->getArgumentType())
                        {
                            case Argument::A:
                            case Argument::S:
                                size = 1;
                                break;
                            case Argument::IMMEDIATE:
                                argument->forceZeroPage();
                                size = 2;
                                break;
                            case Argument::DIRECT:
                            case Argument::INDEXED_BY_Y:
                                argument->checkForZeroPage();
                                size = argument->isZeroPage() ? 2 : 3;
                                break;
                            default:
                                commandError("if receiver is the register `x`, then the argument must be the register `a` or `s`, an immediate value #foo, or a direct memory term of form @foo or @foo[y]");
                                break;
                        }
                        break;
                    case Argument::Y:
                        switch(argument->getArgumentType())
                        {
                            case Argument::A:
                                size = 1;
                                break;
                            case Argument::IMMEDIATE:
                                argument->forceZeroPage();
                                size = 2;
                                break;
                            case Argument::DIRECT:
                            case Argument::INDEXED_BY_X:
                                argument->checkForZeroPage();
                                size = argument->isZeroPage() ? 2 : 3;
                                break;
                            default:
                                commandError("if receiver is the register `y`, then the argument must be the register `a`, an immediate value #foo, or a direct memory term of form @foo or @foo[x]");
                                break;
                        }
                        break;
                    case Argument::S:
                        switch(argument->getArgumentType())
                        {
                            case Argument::X:
                                size = 1;
                                break;
                            default:
                                commandError("if receiver is the register `x`, then the argument must be `x`");
                                break;
                        }
                        break;
                    case Argument::DIRECT:
                    case Argument::INDEXED_BY_X:
                    case Argument::INDEXED_BY_Y:
                    case Argument::ZP_INDEXED_INDIRECT:
                    case Argument::ZP_INDIRECT_INDEXED:
                    {
                        // Swap receiver and argument, and make this a put.
                        Argument* temp = receiver;
                        receiver = argument;
                        argument = temp;
                        
                        oldCommandType = commandType;
                        commandType = PUT;
                        // Recursive call to repeat instruction size selection.
                        // Thankfully only one-deep.
                        return calculateSize();
                    }
                    default:
                        commandError("receiver must be the register `a`, `x`, `y`, or `s`, or some memory term that is not an immediate value");
                        break;
                }
                break;  
            case PUT:
                switch(receiver->getArgumentType())
                {
                    case Argument::A:
                        switch(argument->getArgumentType())
                        {
                            case Argument::X:
                            case Argument::Y:
                                size = 1;
                                break;
                            case Argument::ZP_INDEXED_INDIRECT:
                            case Argument::ZP_INDIRECT_INDEXED:
                                argument->forceZeroPage();
                                size = 2;
                                break;
                            case Argument::DIRECT:
                            case Argument::INDEXED_BY_X:
                                argument->checkForZeroPage();
                                size = argument->isZeroPage() ? 2 : 3;
                                break;
                            case Argument::INDEXED_BY_Y:
                                size = 3;
                                break;
                            default:
                                commandError("if receiver is the register `a`, then the argument must be the register `x` or `y`, a direct memory term of form @foo, @foo[x] or @foo[y], or an indirect term of form @[foo[x]] or @[foo][y]");
                                break;
                        }
                        break;
                    case Argument::X:
                        switch(argument->getArgumentType())
                        {
                            case Argument::A:
                            case Argument::S:
                                size = 1;
                                break;
                            case Argument::INDEXED_BY_Y:
                                // Assume zero-page since there is no absolute direct-index mode for stx.
                                argument->forceZeroPage();
                                size = 2;
                                break;
                            case Argument::DIRECT:
                                argument->checkForZeroPage();
                                size = argument->isZeroPage() ? 2 : 3;
                                break;
                            default:
                                commandError("if receiver is the register `x`, then the argument must be the register `a` or `s`, or a direct memory term of form @foo or @foo[y]");
                                break;
                        }
                        break;
                    case Argument::Y:
                        switch(argument->getArgumentType())
                        {
                            case Argument::A:
                                size = 1;
                                break;
                            case Argument::INDEXED_BY_X:
                                // Assume zero-page since there is no absolute direct-index mode for sty.
                                argument->forceZeroPage();
                                size = 2;
                                break;
                            case Argument::DIRECT:
                                argument->checkForZeroPage();
                                size = argument->isZeroPage() ? 2 : 3;
                                break;
                            default:
                                commandError("if receiver is the register `y`, then the argument must be the register `a`, or a direct memory term of form @foo or @foo[x]");
                                break;
                        }
                        break;
                    case Argument::S:
                        switch(argument->getArgumentType())
                        {
                            case Argument::X:
                                size = 1;
                                break;
                            default:
                                commandError("if receiver is the register `x`, then the argument must be `x`");
                                break;
                        }
                        break;
                    case Argument::IMMEDIATE:
                    case Argument::DIRECT:
                    case Argument::INDEXED_BY_X:
                    case Argument::INDEXED_BY_Y:
                    case Argument::ZP_INDEXED_INDIRECT:
                    case Argument::ZP_INDIRECT_INDEXED:
                    {
                        // Swap receiver and argument, and make this a get.
                        Argument* temp = receiver;
                        receiver = argument;
                        argument = temp;
                        
                        oldCommandType = commandType;
                        commandType = GET;
                        // Recursive call to repeat instruction size selection.
                        // Thankfully only one-deep.
                        return calculateSize();
                    }
                    default:
                        commandError("receiver must be the register `a`, `x`, `y`, or `s`, or some memory term");
                        break;
                }
                break;
            case CMP:
                switch(receiver->getArgumentType())
                {
                    case Argument::A:
                        switch(argument->getArgumentType())
                        {
                            case Argument::IMMEDIATE:
                            case Argument::ZP_INDEXED_INDIRECT:
                            case Argument::ZP_INDIRECT_INDEXED:
                                argument->forceZeroPage();
                                size = 2;
                                break;
                            case Argument::DIRECT:
                            case Argument::INDEXED_BY_X:
                                argument->checkForZeroPage();
                                size = argument->isZeroPage() ? 2 : 3;
                                break;
                            case Argument::INDEXED_BY_Y:
                                size = 3;
                                break;
                            default:
                                commandError("if receiver is the register `a`, then the argument must be an immediate value #foo, a direct memory term of form @foo, @foo[x] or @foo[y], or an indirect term of form @[foo[x]] or @[foo][y]");
                                break;
                        }
                        break;
                    case Argument::X:
                    case Argument::Y:
                        switch(argument->getArgumentType())
                        {
                            case Argument::IMMEDIATE:
                                argument->forceZeroPage();
                                size = 2;
                                break;
                            case Argument::DIRECT:
                                argument->checkForZeroPage();
                                size = argument->isZeroPage() ? 2 : 3;
                                break;
                            default:
                                commandError("if receiver is an index register, then the argument must be an immediate value #foo, or a direct memory term of form @foo");
                                break;
                        }
                        break;
                    default:
                        commandError("receiver must be the register `a`, `x`, or `y`.");
                        break;
                }
                break;
            case ADD:
            case ADDC:
            case SUB:
            case SUBC:
            case BITWISE_OR:
            case BITWISE_AND:
            case BITWISE_XOR:
                if(receiver->getArgumentType() == Argument::A)
                {
                    switch(argument->getArgumentType())
                    {
                        case Argument::IMMEDIATE:
                        case Argument::ZP_INDEXED_INDIRECT:
                        case Argument::ZP_INDIRECT_INDEXED:
                            argument->forceZeroPage();
                            size = 2;
                            break;
                        case Argument::DIRECT:
                        case Argument::INDEXED_BY_X:
                            argument->checkForZeroPage();
                            size = argument->isZeroPage() ? 2 : 3;
                            break;
                        case Argument::INDEXED_BY_Y:
                            size = 3;
                            break;
                        default:
                            commandError("argument must be an immediate value #foo, a direct memory term of form @foo, @foo[x] or @foo[y], or an indirect term of form @[foo[x]] or @[foo][y]");
                            break;
                    }
                    if(commandType == ADD || commandType == SUB)
                    {
                        // add size of clc / sec {+1}
                        size += 1;
                    }
                }
                else
                {
                    commandError("receiver must be the register `a`.");
                }
                break;
            case BIT:
                if(receiver->getArgumentType() == Argument::A)
                {
                    switch(argument->getArgumentType())
                    {
                        case Argument::DIRECT:
                            argument->checkForZeroPage();
                            size = argument->isZeroPage() ? 2 : 3;
                            break;
                        default:
                            commandError("argument must be a direct memory term of form @foo");
                            break;
                    }
                }
                else
                {
                    commandError("receiver must be the register `a`.");
                }
                break;
            // This has X, Y or a memory term as a receiver. No argument.
            case INC:
            case DEC:
                switch(receiver->getArgumentType())
                {
                    case Argument::X:
                    case Argument::Y:
                        size = 1;
                        break;
                    case Argument::DIRECT:
                    case Argument::INDEXED_BY_X:
                        receiver->checkForZeroPage();
                        size = receiver->isZeroPage() ? 2 : 3;
                        break;
                    default:
                        commandError("receiver must be the register `x`, register `y`, or a direct memory term of form @foo, or @foo[x].");
                        break;
                }
                break;
            // This has A as a receiver. No argument.
            case NOT:
                if(receiver->getArgumentType() == Argument::A)
                {
                    // size of eor #0xff {+2}
                    size = 2;
                }
                else
                {
                    commandError("receiver must be the register `a`.");
                }
                break;
            // This has A as a receiver. No argument.
            case NEG:
                if(receiver->getArgumentType() == Argument::A)
                {
                    // size of clc {+1}, eor #0xff {+2}, adc #01 {+2}
                    size = 5;
                }
                else
                {
                    commandError("receiver must be the register `a`.");
                }
                break;
            // These have A or a memory term as a receiver. No argument.
            case SHL:
            case SHR:
            case ROL:
            case ROR:
                switch(receiver->getArgumentType())
                {
                    case Argument::A:
                        size = 1;
                        break;
                    case Argument::DIRECT:
                    case Argument::INDEXED_BY_X:
                        receiver->checkForZeroPage();
                        size = receiver->isZeroPage() ? 2 : 3;
                        break;
                    default:
                        commandError("receiver must be the register `a`, or a direct memory term of form @expr, or @expr[x].");
                        break;
                }
                break;
            // These have A or P as a receiver.
            case PUSH:
            case PULL:
                switch(receiver->getArgumentType())
                {
                    case Argument::A:
                    case Argument::P:
                        size = 1;
                        break;
                    default:
                        commandError("receiver must be the register `a` or `p`.");
                        break;
                }
                break;
            // These require a p-flag argument (error if there is none)
            case SET:
                if(receiver->getArgumentType() == Argument::P)
                {
                    switch(argument->getArgumentType())
                    {
                        case Argument::CARRY:
                        case Argument::INTERRUPT:
                        case Argument::DECIMAL:
                            size = 1;
                            break;
                        default:
                            commandError("argument must be the p-flag `carry`, `interrupt`, or `decimal`.");
                            break;
                    }
                }
                else
                {
                    commandError("receiver must be the register `p`.");
                }
                break;
            case UNSET:
                if(receiver->getArgumentType() == Argument::P)
                {
                    switch(argument->getArgumentType())
                    {
                        case Argument::CARRY:
                        case Argument::INTERRUPT:
                        case Argument::DECIMAL:
                        case Argument::OVERFLOW:
                            size = 1;
                            break;
                        default:
                            commandError("argument must be the p-flag `carry`, `interrupt`, `decimal`, or `overflow`.");
                            break;
                    }
                }
                else
                {
                    commandError("receiver must be the register `p`.");
                }
                break;
        }
        return size;
    }
    
    void Command::write(RomBank* bank)
    {
        bool defaultAssembly = true;
        unsigned int opcode = 0;
        switch(commandType)
        {
            case GET:
                switch(receiver->getArgumentType())
                {
                    case Argument::A:
                        switch(argument->getArgumentType())
                        {
                            case Argument::X:
                                opcode = 0x8A; // txa
                                break;
                            case Argument::Y:
                                opcode = 0x98; // tya
                                break;
                            case Argument::IMMEDIATE:
                                opcode = 0xA9; // lda #imm
                                break;
                            case Argument::ZP_INDEXED_INDIRECT:
                                opcode = 0xA1; // lda [mem, x]
                                break;
                            case Argument::ZP_INDIRECT_INDEXED:
                                opcode = 0xB1; // lda [mem], y
                                break;
                            case Argument::DIRECT:
                                opcode = argument->isZeroPage() ? 0xA5 : 0xAD; // lda mem
                                break;
                            case Argument::INDEXED_BY_X:
                                opcode = argument->isZeroPage() ? 0xB5 : 0xBD; // lda mem, x
                                break;
                            case Argument::INDEXED_BY_Y:
                                opcode = 0xB9; // lda mem, y
                                break;
                        }
                        break;
                    case Argument::X:
                        switch(argument->getArgumentType())
                        {
                            case Argument::A:
                                opcode = 0xAA; // tax
                                break;
                            case Argument::S:
                                opcode = 0xBA; // tsx
                                break;
                            case Argument::IMMEDIATE:
                                opcode = 0xA2; // ldx #imm
                                break;
                            case Argument::DIRECT:
                                opcode = argument->isZeroPage() ? 0xA6 : 0xAE; // ldx mem
                                break;
                            case Argument::INDEXED_BY_Y:
                                opcode = argument->isZeroPage() ? 0xB4 : 0xBE; // ldx mem, y
                                break;
                        }
                        break;
                    case Argument::Y:
                        switch(argument->getArgumentType())
                        {
                            case Argument::A:
                                opcode = 0xA8; // tay
                                break;
                            case Argument::IMMEDIATE:
                                opcode = 0xA0; // ldy #imm
                                break;
                            case Argument::DIRECT:
                                opcode = argument->isZeroPage() ? 0xA4 : 0xAC; // ldy mem
                                break;
                            case Argument::INDEXED_BY_X:
                                opcode = argument->isZeroPage() ? 0xB4 : 0xBC; // ldy mem, x
                                break;
                        }
                        break;
                    case Argument::S:
                        switch(argument->getArgumentType())
                        {
                            case Argument::X:
                                opcode = 0x9A; // txs
                                break;
                        }
                        break;
                }
                break;
            case PUT:
                switch(receiver->getArgumentType())
                {
                    case Argument::A:
                        switch(argument->getArgumentType())
                        {
                            case Argument::X:
                                opcode = 0xAA; // tax
                                break;
                            case Argument::Y:
                                opcode = 0xA8; // tay
                                break;
                            case Argument::ZP_INDEXED_INDIRECT:
                                opcode = 0x81; // sta [mem, x]
                                break;
                            case Argument::ZP_INDIRECT_INDEXED:
                                opcode = 0x91; // sta [mem], y
                                break;
                            case Argument::DIRECT:
                                opcode = argument->isZeroPage() ? 0x85: 0x8D; // sta mem
                                break;
                            case Argument::INDEXED_BY_X:
                                opcode = argument->isZeroPage() ? 0x95: 0x9D; // sta mem, x
                                break;
                            case Argument::INDEXED_BY_Y:
                                opcode = 0x99; // sta mem, y
                                break;
                        }
                        break;
                    case Argument::X:
                        switch(argument->getArgumentType())
                        {
                            case Argument::A:
                                opcode = 0x8A; // txa
                                break;
                            case Argument::S:
                                opcode = 0x9A; // txs
                                break;
                            case Argument::INDEXED_BY_Y:
                                opcode = 0x96; // stx zp, y
                                break;
                            case Argument::DIRECT:
                                opcode = argument->isZeroPage() ? 0x86 : 0x8E; // stx mem
                                break;
                        }
                        break;
                    case Argument::Y:
                        switch(argument->getArgumentType())
                        {
                            case Argument::A:
                                opcode = 0x98; // tya
                                break;
                            case Argument::INDEXED_BY_X:
                                opcode = 0x94; // sty zp, x
                                break;
                            case Argument::DIRECT:
                                opcode = argument->isZeroPage() ? 0x84 : 0x8C; // sty mem
                                break;
                        }
                        break;
                    case Argument::S:
                        switch(argument->getArgumentType())
                        {
                            case Argument::X:
                                opcode = 0xBA; // tsx
                                break;
                        }
                        break;
                }
                break;
            case CMP:
                switch(receiver->getArgumentType())
                {
                    case Argument::A:
                        switch(argument->getArgumentType())
                        {
                            case Argument::IMMEDIATE:
                                opcode = 0xC9; // cmp #imm
                                break;
                            case Argument::ZP_INDEXED_INDIRECT:
                                opcode = 0xC1; // cmp [mem, x]
                                break;
                            case Argument::ZP_INDIRECT_INDEXED:
                                opcode = 0xD1; // cmp [mem], y
                                break;
                            case Argument::DIRECT:
                                opcode = argument->isZeroPage() ? 0xC5 : 0xCD; // cmp mem
                                break;
                            case Argument::INDEXED_BY_X:
                                opcode = argument->isZeroPage() ? 0xD5 : 0xDD; // cmp mem, x
                                break;
                            case Argument::INDEXED_BY_Y:
                                opcode = 0xD1; // cmp mem, y
                                break;
                        }
                        break;
                    case Argument::X:
                        switch(argument->getArgumentType())
                        {
                            case Argument::IMMEDIATE:
                                opcode = 0xE0; // cpx #imm
                                break;
                            case Argument::DIRECT:
                                opcode = argument->isZeroPage() ? 0xE4 : 0xEC; // cpx mem
                                break;
                        }
                        break;
                    case Argument::Y:
                        switch(argument->getArgumentType())
                        {
                            case Argument::IMMEDIATE:
                                opcode = 0xC0; // cpy #imm
                                break;
                            case Argument::DIRECT:
                                opcode = argument->isZeroPage() ? 0xC4 : 0xCC; // cpy mem
                                break;
                        }
                        break;
                }
                break;
            case ADD:
                bank->writeByte(0x18, getSourcePosition()); // clc
                // fallthrough for adc
            case ADDC:
                switch(argument->getArgumentType())
                {
                    case Argument::IMMEDIATE:
                        opcode = 0x69; // adc #imm
                        break;
                    case Argument::ZP_INDEXED_INDIRECT:
                        opcode = 0x61; // adc [mem, x]
                        break;
                    case Argument::ZP_INDIRECT_INDEXED:
                        opcode = 0x71; // adc [mem], y
                        break;
                    case Argument::DIRECT:
                        opcode = argument->isZeroPage() ? 0x65 : 0x6D; // adc mem
                        break;
                    case Argument::INDEXED_BY_X:
                        opcode = argument->isZeroPage() ? 0x75 : 0x7D; // adc mem, x
                        break;
                    case Argument::INDEXED_BY_Y:
                        opcode = 0x79; // adc mem, y
                        break;
                }
                break;
            case SUB:
                bank->writeByte(0x38, getSourcePosition()); // secc
                // fallthrough for sbc
            case SUBC:
                switch(argument->getArgumentType())
                {
                    case Argument::IMMEDIATE:
                        opcode = 0xE9; // sbc #imm
                        break;
                    case Argument::ZP_INDEXED_INDIRECT:
                        opcode = 0xE1; // sbc [mem, x]
                        break;
                    case Argument::ZP_INDIRECT_INDEXED:
                        opcode = 0xF1; // sbc [mem], y
                        break;
                    case Argument::DIRECT:
                        opcode = argument->isZeroPage() ? 0xE5 : 0xED; // sbc mem
                        break;
                    case Argument::INDEXED_BY_X:
                        opcode = argument->isZeroPage() ? 0xF5 : 0xFD; // sbc mem, x
                        break;
                    case Argument::INDEXED_BY_Y:
                        opcode = 0xF9; // sbc mem, y
                        break;
                }
                break;
            case BITWISE_OR:
                switch(argument->getArgumentType())
                {
                    case Argument::IMMEDIATE:
                        opcode = 0x09; // ora #imm
                        break;
                    case Argument::ZP_INDEXED_INDIRECT:
                        opcode = 0x01; // ora [mem, x]
                        break;
                    case Argument::ZP_INDIRECT_INDEXED:
                        opcode = 0x11; // ora [mem], y
                        break;
                    case Argument::DIRECT:
                        opcode = argument->isZeroPage() ? 0x05 : 0x0D; // ora mem
                        break;
                    case Argument::INDEXED_BY_X:
                        opcode = argument->isZeroPage() ? 0x15 : 0x1D; // ora mem, x
                        break;
                    case Argument::INDEXED_BY_Y:
                        opcode = 0x19; // ora mem, y
                        break;
                }
                break;
            case BITWISE_AND:
                switch(argument->getArgumentType())
                {
                    case Argument::IMMEDIATE:
                        opcode = 0x29; // and #imm
                        break;
                    case Argument::ZP_INDEXED_INDIRECT:
                        opcode = 0x21; // and [mem, x]
                        break;
                    case Argument::ZP_INDIRECT_INDEXED:
                        opcode = 0x31; // and [mem], y
                        break;
                    case Argument::DIRECT:
                        opcode = argument->isZeroPage() ? 0x25 : 0x2D; // and mem
                        break;
                    case Argument::INDEXED_BY_X:
                        opcode = argument->isZeroPage() ? 0x35 : 0x3D; // and mem, x
                        break;
                    case Argument::INDEXED_BY_Y:
                        opcode = 0x39; // and mem, y
                        break;
                }
                break;
            case BITWISE_XOR:
                switch(argument->getArgumentType())
                {
                    case Argument::IMMEDIATE:
                        opcode = 0x49; // eor #imm
                        break;
                    case Argument::ZP_INDEXED_INDIRECT:
                        opcode = 0x41; // eor [mem, x]
                        break;
                    case Argument::ZP_INDIRECT_INDEXED:
                        opcode = 0x51; // eor [mem], y
                        break;
                    case Argument::DIRECT:
                        opcode = argument->isZeroPage() ? 0x45 : 0x4D; // eor mem
                        break;
                    case Argument::INDEXED_BY_X:
                        opcode = argument->isZeroPage() ? 0x55 : 0x5D; // eor mem, x
                        break;
                    case Argument::INDEXED_BY_Y:
                        opcode = 0x59; // eor mem, y
                        break;
                }
                break;
            case BIT:
                switch(argument->getArgumentType())
                {
                    case Argument::DIRECT:
                        opcode = argument->isZeroPage() ? 0x24 : 0x2C; // bit mem
                        break;
                }
                break;
            case INC:
                switch(receiver->getArgumentType())
                {
                    case Argument::X:
                        opcode = 0xE8; // inx
                        break;
                    case Argument::Y:
                        opcode = 0xC8; // iny
                        break;
                    case Argument::DIRECT:
                        opcode = receiver->isZeroPage() ? 0xE6 : 0xEE; // inc mem
                        break;
                    case Argument::INDEXED_BY_X:
                        opcode = receiver->isZeroPage() ? 0xF6 : 0xFE; // inc mem, x
                        break;
                }
                break;
            case DEC:
                switch(receiver->getArgumentType())
                {
                    case Argument::X:
                        opcode = 0xCA; // dex
                        break;
                    case Argument::Y:
                        opcode = 0x88; // dey
                        break;
                    case Argument::DIRECT:
                        opcode = receiver->isZeroPage() ? 0xC6 : 0xCE; // dec mem
                        break;
                    case Argument::INDEXED_BY_X:
                        opcode = receiver->isZeroPage() ? 0xD6 : 0xDE; // dec mem, x
                        break;
                }
                break;
            case SHL:
                switch(receiver->getArgumentType())
                {
                    case Argument::A:
                        opcode = 0x0A; // asl a
                        break;
                    case Argument::DIRECT:
                        opcode = receiver->isZeroPage() ? 0x06 : 0x0E; // asl mem
                        break;
                    case Argument::INDEXED_BY_X:
                        opcode = receiver->isZeroPage() ? 0x16 : 0x1E; // asl mem, x
                        break;
                }
                break;
            case SHR:
                switch(receiver->getArgumentType())
                {
                    case Argument::A:
                        opcode = 0x4A; // lsr a
                        break;
                    case Argument::DIRECT:
                        opcode = receiver->isZeroPage() ? 0x46 : 0x4E; // lsr mem
                        break;
                    case Argument::INDEXED_BY_X:
                        opcode = receiver->isZeroPage() ? 0x56 : 0x5E; // lsr mem, x
                        break;
                }
                break;
            case ROL:
                switch(receiver->getArgumentType())
                {
                    case Argument::A:
                        opcode = 0x2A; // rol a
                        break;
                    case Argument::DIRECT:
                        opcode = receiver->isZeroPage() ? 0x26 : 0x2E; // rol mem
                        break;
                    case Argument::INDEXED_BY_X:
                        opcode = receiver->isZeroPage() ? 0x36 : 0x3E; // rol mem, x
                        break;
                }
                break;
            case ROR:
                switch(receiver->getArgumentType())
                {
                    case Argument::A:
                        opcode = 0x6A; // ror a
                        break;
                    case Argument::DIRECT:
                        opcode = receiver->isZeroPage() ? 0x66 : 0x6E; // ror mem
                        break;
                    case Argument::INDEXED_BY_X:
                        opcode = receiver->isZeroPage() ? 0x76 : 0x7E; // ror mem, x
                        break;
                }
                break;
            case PUSH:
                switch(receiver->getArgumentType())
                {
                    case Argument::A:
                        opcode = 0x48; // pha
                    case Argument::P:
                        opcode = 0x08; // php
                        break;
                }
                break;
            case PULL:
                switch(receiver->getArgumentType())
                {
                    case Argument::A:
                        opcode = 0x68; // pla
                    case Argument::P:
                        opcode = 0x28; // plp
                        break;
                }
                break;
            case SET:
                switch(argument->getArgumentType())
                {
                    case Argument::CARRY:
                        opcode = 0x38; // sec
                        break;
                    case Argument::INTERRUPT:
                        opcode = 0x78; // sei
                        break;
                    case Argument::DECIMAL:
                        opcode = 0xF8; // sed
                        break;
                }
                break;
            case UNSET:
                switch(argument->getArgumentType())
                {
                    case Argument::CARRY:
                        opcode = 0x18; // clc
                        break;
                    case Argument::INTERRUPT:
                        opcode = 0x58; // cli
                        break;
                    case Argument::DECIMAL:
                        opcode = 0xD8; // cld
                        break;
                    case Argument::OVERFLOW:
                        opcode = 0xB8; // clv
                        break;
                }
                break;
            case NOT:
                defaultAssembly = false;
                bank->writeByte(0x49, getSourcePosition()); // eor #imm
                bank->writeByte(0xFF, getSourcePosition()); // with imm = 0xff
                break;
            case NEG:
                defaultAssembly = false;
                bank->writeByte(0x18, getSourcePosition()); // clc
                bank->writeByte(0x49, getSourcePosition()); // eor #imm
                bank->writeByte(0xFF, getSourcePosition()); // with imm = 0xff
                bank->writeByte(0x69, getSourcePosition()); // adc #imm
                bank->writeByte(0x01, getSourcePosition()); // with imm = 0x01
                break;
        }
        
        if(defaultAssembly)
        {
            if(opcode)
            {
                // Write opcode byte.
                bank->writeByte(opcode, getSourcePosition());
                // Write receiver or argument, only one of which will have non-zero size.
                receiver->write(bank);
                if(argument)
                {
                    argument->write(bank);
                }
            }
            else
            {
                error("internal: output not generated for command", getSourcePosition(), true);
            }
        }
    }
}