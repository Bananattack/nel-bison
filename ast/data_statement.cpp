#include "error.h"
#include "rom_generator.h"
#include "rom_bank.h"
#include "data_statement.h"

namespace nel
{
    DataStatement::DataStatement(DataType dataType, ListNode<DataItem*>* items, SourcePosition* sourcePosition)
        : Statement(Statement::DATA, sourcePosition), dataType(dataType), items(items)
    {
    }

    DataStatement::~DataStatement()
    {
        delete items;
    }
    
    void DataStatement::aggregate()
    {
    }

    void DataStatement::validate()
    {
        unsigned int baseSize = dataType == WORD ? 2 : 1;
        unsigned int size = 0;
        
        ListNode<DataItem*>::ListType& list = items->getList();
        
        for(size_t i = 0; i < list.size(); i++)
        {
            DataItem* item = list[i];
            
            item->check();
            switch(item->getItemType())
            {
                case DataItem::STRING_LITERAL:
                    size += baseSize * item->getStringLiteral()->getValue().length();
                    break;
                case DataItem::EXPRESSION:
                    size += baseSize;
                    break;
            }
        }
        
        // Reserve the bytes needed for this data.
        // (Previous errors shouldn't wreck the size calculation).
        RomBank* bank = romGenerator->getActiveBank();
        if(!bank)
        {
            error("data statement found, but a rom bank hasn't been selected yet", getSourcePosition(), true);
        }
        else
        {
            bank->expand(size, getSourcePosition());
        }
    }
    
    void DataStatement::generate()
    {
        // Get the bank to use for writing.
        RomBank* bank = romGenerator->getActiveBank();
        if(!bank)
        {
            error("data statement found, but a rom bank hasn't been selected yet", getSourcePosition(), true);
            return;
        }
        
        ListNode<DataItem*>::ListType list = items->getList();
        
        for(size_t i = 0; i < list.size(); i++)
        {
            DataItem* item = list[i];

            switch(item->getItemType())
            {
                case DataItem::STRING_LITERAL:
                {
                    const std::string& str = item->getStringLiteral()->getValue();
                    const char* data = str.data();
                    if(dataType == WORD)
                    {
                        for(size_t j = 0; j < str.length(); j++)
                        {
                            bank->writeWord(data[i], getSourcePosition());
                        }
                    }
                    else
                    {
                        for(size_t j = 0; j < str.length(); j++)
                        {
                            bank->writeByte(data[i], getSourcePosition());
                        }                    
                    }
                    break;
                }
                case DataItem::EXPRESSION:
                    if(item->getExpression()->fold(true, true))
                    {
                        if(dataType == WORD)
                        {
                            bank->writeWord(item->getExpression()->getFoldedValue(), getSourcePosition());
                        }
                        else
                        {   
                            bank->writeByte(item->getExpression()->getFoldedValue(), getSourcePosition());
                        }
                    }
                    else
                    {
                        error("data item has indeterminate value", getSourcePosition());
                    }
                    break;
            }
        }
    }
}