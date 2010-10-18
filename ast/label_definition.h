#pragma once

#include "definition.h"
#include "label_declaration.h"

namespace nel
{
    /**
     * This represents a Definition that pertains to a label,
     * which is created upon that label's declaration.
     */
    class LabelDefinition : public Definition
    {
        private:
            LabelDeclaration* labelDeclaration;
            bool locationKnown;
            unsigned int location;
            
        public:    
            LabelDefinition(std::string name, LabelDeclaration* labelDeclaration)
                : Definition(Definition::LABEL, name), labelDeclaration(labelDeclaration), locationKnown(false)
            {
            }
            
            /**
             * Get the declaration associated with this definition.
             */
            LabelDeclaration* getLabelDeclaration()
            {
                return labelDeclaration;
            }
            
            /**
             * Returns whether or not the label's location is known.
             */
            bool isLocationKnown()
            {
                return locationKnown;
            }
            
            /**
             * Get the location of this label.
             * Value is undefined if isLocationKnown() is false.
             */
            unsigned int getLocation()
            {
                return locationKnown ? location : 0xCACAFACE;
            }
            
            /**
             * Modify the location of this label.
             * When set, isLocationKnown() will return true.
             */
            void setLocation(unsigned int location)
            {
                this->location = location;
                locationKnown = true;
            }

    };
}