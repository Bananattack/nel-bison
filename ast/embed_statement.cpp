#include <fstream>
#include <sstream>

#include "error.h"
#include "rom_generator.h"
#include "rom_bank.h"
#include "embed_statement.h"

namespace nel
{
    EmbedStatement::EmbedStatement(StringNode* filename, SourcePosition* sourcePosition)
        : Statement(Statement::EMBED, sourcePosition), filename(filename), filesize(0)
    {
    }

    EmbedStatement::~EmbedStatement()
    {
        delete filename;
    }
    
    void EmbedStatement::aggregate()
    {
    }

    void EmbedStatement::validate()
    {   
        std::ifstream file(filename->getValue().c_str(), std::ios::in | std::ios::binary);
        
        if(file.good() && file.is_open())
        {
            file.seekg(0, std::ios_base::beg);
            std::ifstream::pos_type start = file.tellg();
            file.seekg(0, std::ios_base::end);
            filesize = file.tellg() - start;
            file.close();
        }
        else
        {
            std::ostringstream os;
            os << "could not open file `" << filename->getValue() << "` required by an embed statement";
            error(os.str(), getSourcePosition(), true);
            return;
        }
        
        // Reserve the bytes needed for this data.
        // (Previous errors shouldn't wreck the size calculation).
        RomBank* bank = romGenerator->getActiveBank();
        if(!bank)
        {
            error("embed statement found, but a rom bank hasn't been selected yet", getSourcePosition(), true);
        }
        else
        {
            bank->expand(filesize, getSourcePosition());
        }
    }

    void EmbedStatement::generate()
    {
        // Get the bank to write into.
        RomBank* bank = romGenerator->getActiveBank();
        if(!bank)
        {
            error("embed statement found, but a rom bank hasn't been selected yet", getSourcePosition(), true);
            return;
        }
        
        std::ifstream file(filename->getValue().c_str(), std::ios::in | std::ios::binary);
        
        if(file.good() && file.is_open())
        {
            std::vector<char> bytes(filesize);
            file.read(&bytes[0], filesize);
            file.close();
            
            for(size_t i = 0; i < bytes.size(); i++)
            {
                bank->writeByte((unsigned char) bytes[i], getSourcePosition());
            }
        }
        else
        {
            std::ostringstream os;
            os << "could not open file `" << filename->getValue() << " required by an embed statement";
            error(os.str(), getSourcePosition(), true);
            return;
        }
    }
    
}