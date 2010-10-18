#pragma once

#include "set.h"
#include "map.h"
#include "statement.h"
#include "list_node.h"
#include "header_setting.h"

namespace nel
{
    /**
     * A statement which defines a full header for the ROM.
     * The information is both used to construct a valid iNES header,
     * and to allocate ROM storage for PRG and CHR banks.
     */
    class HeaderStatement : public Statement
    {
        private:
            ListNode<HeaderSetting*>* settings;
            typedef Set<std::string>::Type StringSet;
            typedef Map<std::string, HeaderSetting*>::Type SettingTable;
            
            static StringSet& getRecognizedSettings();
        public:
            HeaderStatement(ListNode<HeaderSetting*>* settings, SourcePosition* sourcePosition);
            ~HeaderStatement();
            
        private:
            HeaderSetting* findSetting(SettingTable& settingTable, std::string name, bool optional = false);
            
        public:
            /**
             * Returns a list of all settings passed to the header statement.
             */
            ListNode<HeaderSetting*>* getSettings()
            {
                return settings;
            }

            void aggregate();
            void validate();
            void generate();
    };
}