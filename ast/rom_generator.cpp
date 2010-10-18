#include <iostream>
#include <sstream>

#include "error.h"
#include "rom_generator.h"

namespace nel
{
    RomGenerator* romGenerator;
    
    RomGenerator::RomGenerator(unsigned int mapper, unsigned int prg, unsigned int chr, bool mirroring, bool battery, bool fourscreen)
        : mapper(mapper), prg(prg), chr(chr), mirroring(mirroring), battery(battery), fourscreen(fourscreen), bankSet(false), ramCounterSet(false)
    {
        for(unsigned int i = 0; i < prg * 2 + chr; i++)
        {
            banks.push_back(new RomBank());
        }
    }

    RomGenerator::~RomGenerator()
    {
        for(size_t i = 0; i < banks.size(); i++)
        {
            delete banks[i];
        }
    }
    
    void RomGenerator::resetRomPosition()
    {
        for(size_t i = 0; i < banks.size(); i++)
        {
            banks[i]->resetPosition();
        }
        bankSet = false;
    }
    
    void RomGenerator::switchBank(unsigned int bankIndex, SourcePosition* sourcePosition)
    {
        if(bankIndex < 0 || bankIndex >= banks.size())
        {
            std::ostringstream os;
            os << "Bank index " << bankIndex << " is outside of range 0.." << banks.size() - 1;
            error(os.str(), sourcePosition, true);
        }
        else
        {
            bankSet = true;
            activeBankIndex = bankIndex;
        }
    }
    
    bool RomGenerator::expandRam(unsigned int size, SourcePosition* sourcePosition)
    {
        if(ramCounter + size > 65536)
        {
            std::ostringstream os;
            os << "ram counter goes past addressable memory 0..65536 by " << (ramCounter + size - 65536) << " bytes";
            error(os.str(), sourcePosition, true);
            return false;
        }
        else
        {
            ramCounter += size;
            return true;
        }
    }
    
    void RomGenerator::debug()
    {
        for(size_t i = 0; i < banks.size(); i++)
        {
            std::cout << "-- bank " << i << " -- ";
            banks[i]->debug();
        }
    }
    
    void RomGenerator::outputToStream(std::ostream& os)
    {
        // Based on info at http://wiki.nesdev.com/w/index.php/INES
        // 0..3: Write "NES" followed by MS-DOS end-of-file
        os.put(0x4E);
        os.put(0x45);
        os.put(0x53);
        os.put(0x1A);
        // 4: Number of 16K PRG ROM banks
        os.put(prg);
        // 5: Number of 8K CHR ROM banks, 0 means this cart has CHR RAM.
        os.put(chr);
        // 6: Write the "Flags 6" byte, skip the 'trainer' flag for now.
        os.put(mirroring | (battery << 1) | (fourscreen << 3) | ((mapper & 0xF) << 4));
        // 7: Write the "Flags 7" byte, just the mapper part though.
        os.put(mapper >> 4);
        // 8: Number of 8K PRG RAM banks -- for now just write a 0, which implies 8KB PRG RAM at most.
        os.put(0);
        // 9: "Flags 9", ignore.
        os.put(0);
        // 10: "Flags 10", ignore.
        os.put(0);
        // 11..15: Zero-pad this header to 16 bytes.
        for(unsigned int i = 0; i < 5; i++)
        {
            os.put(0);
        }
        // Now write everything else.
        for(size_t i = 0; i < banks.size(); i++)
        {
            banks[i]->outputToStream(os);
        }
    }
}