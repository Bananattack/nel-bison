#include <sstream>

#include "error.h"
#include "rom_generator.h"
#include "header_statement.h"

namespace nel
{

    Set<std::string>::Type& HeaderStatement::getRecognizedSettings()
    {
        static Set<std::string>::Type recognizedSettings;
        if(recognizedSettings.empty())
        {
            recognizedSettings.insert("mapper");
            recognizedSettings.insert("prg");
            recognizedSettings.insert("chr");
            recognizedSettings.insert("mirroring");
            recognizedSettings.insert("battery");
            recognizedSettings.insert("fourscreen");
        }
        return recognizedSettings;
    }

    HeaderStatement::HeaderStatement(ListNode<HeaderSetting*>* settings, SourcePosition* sourcePosition)
        : Statement(Statement::HEADER, sourcePosition), settings(settings)
    {
    }
    
    HeaderStatement::~HeaderStatement()
    {
        delete settings;
    }
    
    HeaderSetting* HeaderStatement::findSetting(SettingTable& settingTable, std::string name, bool optional)
    {
        SettingTable::iterator match = settingTable.find(name);
        if(match != settingTable.end())
        {
            return match->second;
        }
        else
        {
            if(!optional)
            {
                std::ostringstream os;
                os << "ines header is missing required setting `" << name << "`";
                error(os.str(), getSourcePosition());
            }
            return 0;
        }
    }
    
    void HeaderStatement::aggregate()
    {
        ListNode<HeaderSetting*>::ListType& list = settings->getList();
        SettingTable settingTable;
        bool headerValid = true;
        
        for(size_t i = 0; i < list.size(); i++)
        {
            const std::string& name = list[i]->getName()->getValue();
            // Try to recognize the name.
            if(getRecognizedSettings().find(name) != getRecognizedSettings().end())
            {
                // See if it already exists in the setting table.
                SettingTable::iterator match = settingTable.find(name);
                // If it does, error.
                if(match != settingTable.end())
                {
                    std::ostringstream os;
                    HeaderSetting* setting = match->second;
                    os << "ines header contains multiple `" << setting->getName()->getValue() <<
                        "` settings, previously declared on " << setting->getSourcePosition() << ".";
                    error(os.str(), list[i]->getSourcePosition());
                    headerValid = false;
                }
                // If it doesn't, you can add it.
                else
                {
                    settingTable[name] = list[i];
                }
            }
            // Otherwise, it is an error.
            else
            {
                std::ostringstream os;
                os << "ines header contains unrecognized header setting `" << name << "`";
                error(os.str(), list[i]->getSourcePosition());
                headerValid = false;
            }
        }       
        
        HeaderSetting* mapper = findSetting(settingTable, "mapper");
        HeaderSetting* prg = findSetting(settingTable, "prg");
        HeaderSetting* chr = findSetting(settingTable, "chr");
        HeaderSetting* mirroring = findSetting(settingTable, "mirroring", true);
        HeaderSetting* battery = findSetting(settingTable, "battery", true);
        HeaderSetting* fourscreen = findSetting(settingTable, "fourscreen", true);
        
        // I use bitwise operators with boolean expressions
        // to ensure that all checks are evaluated (more error reporting).
        // Here, the ternary operator's false case is used to
        // determine whether the header can still be valid if it
        // is missing a particular setting.
        headerValid &= mapper ? mapper->checkValue(0, 255) : false;
        headerValid &= prg ? prg->checkValue(1, 255) : false;
        headerValid &= chr ? chr->checkValue(1, 255) : false;
        headerValid &= mirroring ? mirroring->checkValue(0, 1) : true;
        headerValid &= battery ? battery->checkValue(0, 1) : true;
        headerValid &= fourscreen ? fourscreen->checkValue(0, 1) : true;
        
        if(headerValid)
        {
            romGenerator = new RomGenerator(
                mapper->getExpression()->getFoldedValue(),
                prg->getExpression()->getFoldedValue(),
                chr->getExpression()->getFoldedValue(),
                mirroring ? mirroring->getExpression()->getFoldedValue() != 0 : false,
                battery ? battery->getExpression()->getFoldedValue() != 0 : false,
                fourscreen ? fourscreen->getExpression()->getFoldedValue() != 0 : false
            );
        }
        else
        {
            error("ines header is invalid", getSourcePosition(), true);
        }
    }

    void HeaderStatement::validate()
    {
    }
    
    void HeaderStatement::generate()
    {
    }
    
}