#include <fstream>
#include <sstream>

#include "error.h"
#include "path.h"
#include "rom_generator.h"
#include "rom_bank.h"
#include "embed_statement.h"

namespace nel
{
    EmbedStatement::EmbedStatement(StringNode* relativePath, SourcePosition* sourcePosition)
        : Statement(Statement::EMBED, sourcePosition), relativePath(relativePath), filesize(0)
    {
        filename = getDirectory(sourcePosition->getSourceFile()->getFilename()) + relativePath->getValue();
    }

    EmbedStatement::~EmbedStatement()
    {
        delete relativePath;
    }
    
    void EmbedStatement::aggregate()
    {
    }

    void EmbedStatement::validate()
    {   
        std::ifstream file(filename.c_str(), std::ios::in | std::ios::binary);
        
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
            os << "could not open file `" << filename << "` required by an embed statement";
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
        
        std::ifstream file(filename.c_str(), std::ios::in | std::ios::binary);
        
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
            os << "could not open file `" << filename << " required by an embed statement";
            error(os.str(), getSourcePosition(), true);
            return;
        }
    }
    
}