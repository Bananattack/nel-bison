#include "error.h"
#include "rom_generator.h"
#include "rom_bank.h"
#include "relocation_statement.h"

namespace nel
{
    RelocationStatement::RelocationStatement(RelocationType relocationType, Expression* destinationExpression, SourcePosition* sourcePosition)
        : Statement(Statement::RELOCATION, sourcePosition), relocationType(relocationType), bankExpression(0), destinationExpression(destinationExpression)
    {
    }

    RelocationStatement::RelocationStatement(RelocationType relocationType, Expression* bankExpression, Expression* destinationExpression, SourcePosition* sourcePosition)
        : Statement(Statement::RELOCATION, sourcePosition), relocationType(relocationType), bankExpression(bankExpression), destinationExpression(destinationExpression)
    {
    }

    RelocationStatement::~RelocationStatement()
    {
        delete bankExpression;
        delete destinationExpression;
    }
    
    void RelocationStatement::aggregate()
    {
        if(relocationType == RAM)
        {
            if(destinationExpression)
            {
                if(destinationExpression->fold(true, false))
                {
                    romGenerator->moveRam(destinationExpression->getFoldedValue());
                }
                else
                {
                    error("could not resolve the destination address provided to this ram relocation statement", getSourcePosition(), true);
                }
            }
        }
    }

    void RelocationStatement::validate()
    {
        if(relocationType == ROM)
        {
            if(bankExpression)
            {
                if(bankExpression->fold(true, true))
                {
                    romGenerator->switchBank(bankExpression->getFoldedValue(), getSourcePosition());
                }
                else
                {
                    error("could not resolve the bank number provided to this rom relocation statement", getSourcePosition(), true);
                }
            }
            if(destinationExpression)
            {
                if(destinationExpression->fold(true, true))
                {
                    RomBank* bank = romGenerator->getActiveBank();
                    if(!bank)
                    {
                        error("rom relocation found, but a rom bank hasn't been selected yet", getSourcePosition(), true);
                    }
                    else
                    {
                        bank->org(destinationExpression->getFoldedValue(), getSourcePosition());
                    }
                }
                else
                {
                    error("could not resolve the destination address provided to this rom relocation statement", getSourcePosition(), true);
                }
            }
        }
    }
    
    void RelocationStatement::generate()
    {
        if(relocationType == ROM)
        {
            if(bankExpression)
            {
                romGenerator->switchBank(bankExpression->getFoldedValue(), getSourcePosition());
            }
            
            RomBank* bank = romGenerator->getActiveBank();
            if(!bank)
            {
                error("rom relocation found, but a rom bank hasn't been selected yet", getSourcePosition(), true);
            }
            else
            {
                bank->seekPosition(destinationExpression->getFoldedValue(), getSourcePosition());
            }
        }
    }
}