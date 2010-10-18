#pragma once

#include "statement.h"
#include "list_node.h"
#include "data_item.h"

namespace nel
{
    /**
     * A statement of data which is to be written into the ROM.
     */
    class DataStatement : public Statement
    {   
        public:
            /**
             * An enumeration of possible datatypes that can be emitted.
             */
            enum DataType
            {
                BYTE,   /**< Each data unit takes 1 byte. */
                WORD,   /**< Each data unit takes 2 bytes, padding as necessary. */
            };
        
        private:
            // Size of each element in the listing (always padded as necessary).
            DataType dataType;
            // The data to write, in order.
            ListNode<DataItem*>* items;
            
        public:    
            DataStatement(DataType dataType, ListNode<DataItem*>* items, SourcePosition* sourcePosition);
            ~DataStatement();
            
            /**
             * Returns the datatype that each value uses.
             */
            DataType getDataType()
            {
                return dataType;
            }
            
            /**
             * Returns a list of all the items given by the data.
             */
            ListNode<DataItem*>* getItems()
            {
                return items;
            }

            void aggregate();
            void validate();
            void generate();
    };
}