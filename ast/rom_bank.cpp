#include <cstring>
#include <sstream>
#include <iomanip>

#include "error.h"
#include "rom_bank.h"

namespace nel
{
    RomBank::RomBank()
        : originSet(false), position(0), reservedSize(0)
    {
        memset(data, PAD_VALUE, sizeof(data));
    }
    
    RomBank::~RomBank()
    {
    }
    
    void RomBank::resetPosition()
    {
        position = 0;
    }
    
    void RomBank::expand(unsigned int amount, SourcePosition* sourcePosition)
    {
        if(origin + position + amount > 65536)
        {
            std::ostringstream os;
            os << "bank's position went outside of addressable memory 0..65535 (attempted to expand to position = " << origin + position + amount << ")";
            error(os.str(), sourcePosition, true);
            return;
        }
        
        reservedSize += amount;
        position += amount;
        
        if(!originSet)
        {
            error("no origin point was set before bank was expanded.", sourcePosition, true);
        }
        if(reservedSize > BANK_SIZE)
        {
            std::ostringstream os;
            os << "bank expanded beyond its " << BANK_SIZE << " byte boundary by " << (reservedSize - BANK_SIZE) << " bytes";
            error(os.str(), sourcePosition, true);
        }
    }
    
    void RomBank::org(unsigned int pos, SourcePosition* sourcePosition)
    {
        if(originSet)
        {
            if(pos < origin + position)
            {
                std::ostringstream os;
                os << "attempt to move backwards within the bank. (location " << origin + position << " -> " << pos << ")";
                error(os.str(), sourcePosition, true);
            }
            else
            {   
                expand(pos - (origin + position), sourcePosition);
            }
        }
        else
        {
            origin = pos;
            originSet = true;
        }
    }
    
    void RomBank::seekPosition(unsigned int pos, SourcePosition* sourcePosition)
    {
        if(pos > origin + reservedSize)
        {
            error("attempt to move outside of bank's reserved space.", sourcePosition, true);
        }
        else
        {        
            // Seek to new location.
            position = pos - origin;
        }
    }
    
    
    void RomBank::writeByte(unsigned int value, SourcePosition* sourcePosition)
    {
        if(position >= reservedSize)
        {
            error("attempt to write outside of bank's reserved space.", sourcePosition, true);
        }
        else if(value > 255)
        {
            error("value is outside of representable 16-bit range 0..255", sourcePosition);
        }
        else
        {
            data[position++] = value & 0xFF;
        }
    }

    void RomBank::writeWord(unsigned int value, SourcePosition* sourcePosition)
    {
        if(position >= reservedSize)
        {
            error("attempt to write outside of bank's reserved space.", sourcePosition, true);
        }
        else if(value > 65535)
        {
            error("value is outside of representable 16-bit range 0..65536", sourcePosition);
        }
        else
        {
            std::cout << "PC = ";
            std::cout << std::setfill('0') << std::setw(4) << std::hex << ((int) getProgramCounter());
            std::cout << ": ";
            std::cout << std::setfill('0') << std::setw(2) << std::hex << ((int) value & 0xFF);
            std::cout << " ";
            std::cout << std::setfill('0') << std::setw(2) << std::hex << ((int) (value >> 8) & 0xFF);
            std::cout << std::endl;
            // Write word in little-endian order.
            data[position++] = value & 0xFF;
            data[position++] = (value >> 8) & 0xFF;
        }
    }
    
    void RomBank::debug()
    {
        std::cout << "origin: " << origin << " position: " << position << " reserved: " << reservedSize << " / 8192" << std::endl;
        
        const unsigned int PER_ROW = 32;
        
        for(unsigned int i = 0; i < BANK_SIZE / PER_ROW; i++)
        {
            for(unsigned int j = 0; j < PER_ROW; j++)
            {
                std::cout << std::setfill('0') << std::setw(2) << std::hex << (int) data[i * PER_ROW + j];
                std::cout << " ";
            }
            std::cout << std::endl;
        }
    }
    
    void RomBank::outputToStream(std::ostream& os)
    {
        os.write((char*) data, BANK_SIZE);
    }
}