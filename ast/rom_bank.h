#pragma once

#include "source_position.h"

namespace nel
{
    /**
     * A subsection of memory, with its own internal program counter.
     * This makes it easier to accommodate runtime bank-switches at when using a mapper.
     *
     * Currently banks are fixed to have 8KB boundaries, but at some future point,
     * this could possibly be user-set.
     */
    class RomBank
    {
        private:
            // Size of each ROM bank.
            static const unsigned int BANK_SIZE = 8192;
            // Value used to pad unused bank space.
            static const unsigned char PAD_VALUE = 0xFF;
            
            // Until a ROM relocation occurs, this page has no origin.
            bool originSet;
            // Origin point as an absolute memory address where this bank starts.
            unsigned int origin;
            // The position, in bytes, into the bank.
            unsigned int position;
            // The reserved amount of bytes in this bank, used to
            // error-check bank overflows, and to some extent,
            // to prevent discrepencies between predicted size and actual size.
            unsigned int reservedSize;
            // The byte data held by this bank.
            unsigned char data[BANK_SIZE];
            
        public:
            RomBank();
            ~RomBank();
            
            /**
             * Resets the position within the bank.
             */
            void resetPosition();
            
            /**
             * Returns whether or not this ROM bank's origin is initialized.
             */
            bool hasOrigin()
            {
                return originSet;
            }
            
            /**
             * Returns the current position within the ROM bank.
             * This is a number in the range 0..8191.
             */
            unsigned int getPosition()
            {
                return originSet ? position : 0x12345678;
            }
            
            /**
             * Returns the current value of the program counter,
             * within this bank, within address space 0..65535.
             */
            unsigned int getProgramCounter()
            {
                return originSet ? origin + position : 0xDEAAAAAD;
            }
        
            /**
             * Reserve program space for data in an early pass.
             * Raises an error if the reserved size is > 8KB, or if no origin.
             */
            void expand(unsigned int amount, SourcePosition* sourcePosition);
            
            /**
             * Advance forwards to a new position, and reserve space.
             * If this is the first time, sets the origin point for further advances.
             * This position is an absolute address, converted internally to a relative one.
             * Raises an error if the advance is not legal (backwards), or if expansion fails.
             */
            void org(unsigned int pos, SourcePosition* sourcePosition);
            
            /**
             * For moving around in the bank on later passes.
             * Unlike org, this does not reserve new address space
             * This position is an absolute address.
             * Raises an error if the position is outside reserved space.
             */
            void seekPosition(unsigned int pos, SourcePosition* sourcePosition);
            
            /**
             * Write a single byte.
             * Raises an error if the value is > 255, or if
             * position is outside reserved space.
             */
            void writeByte(unsigned int value, SourcePosition* sourcePosition);
            
            /**
             * Write a word.
             * Raises an error if the value is > 65535, or if
             * position is outside reserved space.
             */
            void writeWord(unsigned int value, SourcePosition* sourcePosition);
            
            
            void debug();
            
            /**
             * Dump the contents of this bank to a binary stream.
             */
            void outputToStream(std::ostream& os);
    };
}