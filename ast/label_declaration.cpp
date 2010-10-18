#include "error.h"
#include "symbol_table.h"
#include "rom_generator.h"
#include "rom_bank.h"
#include "label_declaration.h"
#include "label_definition.h"

namespace nel
{
    LabelDeclaration::LabelDeclaration(StringNode* name, SourcePosition* sourcePosition)
        : Statement(Statement::LABEL_DECLARATION, sourcePosition), name(name), definition(0)
    {
    }
    
    LabelDeclaration::~LabelDeclaration()
    {
        delete name;
    }
    
    void LabelDeclaration::aggregate()
    {
        definition = new LabelDefinition(name->getValue(), this);
        SymbolTable::getActiveScope()->put(definition, name->getSourcePosition());
    }
    
    void LabelDeclaration::validate()
    {
        RomBank* bank = romGenerator->getActiveBank();
        if(bank)
        {
            if(bank->hasOrigin())
            {
                std::cout << "durf" << bank->getProgramCounter() << std::endl;
                definition->setLocation(bank->getProgramCounter());
            }
            else
            {
                error("label declaration was found before the rom location in the current bank was set.", getSourcePosition(), true);
            }
        }
        else
        {
            error("label declaration found, but a rom bank hasn't been selected yet.", getSourcePosition(), true);
        }
    }
    
    void LabelDeclaration::generate()
    {
    }
}