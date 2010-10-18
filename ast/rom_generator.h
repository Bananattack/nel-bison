#include <vector>

#include "rom_bank.h"

namespace nel
{
    class RomGenerator;
    extern RomGenerator* romGenerator;

    class RomGenerator
    {
        private:
            // Mapper number, as designated by the iNES header formate.
            unsigned int mapper;
            // Number of 16K PRG ROM banks.
            unsigned int prg;
            // Number of 8K CHR ROM banks.
            unsigned int chr;
            // Nametable mirroring type (true = vertical / false = horizontal). Defaults to false (horizontal)
            bool mirroring;
            // Has save battery. Defaults to false.
            bool battery;
            // Provides support for four screens of nametables,
            // instead of the system's default of two. Defaults to false.
            bool fourscreen;
            
            // Whether or not the bank index has been specified.
            bool bankSet;
            // The bank of code to use, initialized upon first bank switch.
            unsigned int activeBankIndex;
            // All 8K banks within the ROM. Initialized on construction.
            std::vector<RomBank*> banks;
            
            // The position in RAM. Automatically incremented as variables are defined.
            bool ramCounterSet;
            unsigned int ramCounter;
            
        public:
            RomGenerator(unsigned int mapper, unsigned int prg, unsigned int chr, bool mirroring, bool battery, bool fourscreen);
            ~RomGenerator();
            
            /**
             * Clears the ROM's positional info like the active bank, and resets all banks.
             * This should be called at the start of a new semantic pass.
             */
            void resetRomPosition();
            
            /**
             * Switches to a new bank, and sets its origin. 
             * At least one switch must occur before program code appears.
             */
            void switchBank(unsigned int bankIndex, SourcePosition* sourcePosition);
            
            /**
             * Returns the currently selected bank, or 0 if unset.
             */
            RomBank* getActiveBank()
            {
                return bankSet ? banks[activeBankIndex] : 0;
            }
            
            /**
             * Returns whether or not the ram counter has been initialized.
             */
            bool isRamCounterSet()
            {
                return ramCounterSet;
            }
            
            /**
             * Return current RAM counter. Undefined if not previously set.
             */
            unsigned int getRamCounter()
            {
                return ramCounterSet ? ramCounter : 0xDEADFACE;
            }
            
            /**
             * Relocate RAM counter to new position.
             */
            void moveRam(unsigned int position)
            {
                ramCounterSet = true;
                ramCounter = position;
            }
            
            /**
             * Reserve RAM storage for a variable. Increments ram counter accordingly.
             * Error if RAM counter is uninitialized.
             */
            bool expandRam(unsigned int size, SourcePosition* sourcePosition);
            
            void debug();
            
            /**
             * Dump the generated ROM to a binary stream.
             */
            void outputToStream(std::ostream& os);
    };
}