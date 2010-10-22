#include "error.h"
#include "attribute.h"
#include "symbol_table.h"
#include "block_statement.h"

namespace nel
{
    Attribute::Attribute(ListNode<StringNode*>* pieces, SourcePosition* sourcePosition)
        : Node(sourcePosition), pieces(pieces)
    {
    }

    Attribute::~Attribute()
    {
        delete pieces;   
    }

    Definition* Attribute::findDefinition(bool forbidUndefined)
    {
        ListNode<StringNode*>::ListType& list = pieces->getList();

        StringNode* key = 0;
        Definition* def = 0;
        SymbolTable* scope = SymbolTable::getActiveScope();
        size_t i = 0;
        
        // Check list[0]
        key = list[i];
        def = scope->tryGet(key->getValue());

        // Check list[1] .. list[n-1] (if n > 1).
        for(i = 1; i < list.size(); i++)
        {
            // If a definition exists, then we might be able to index it.
            if(def)
            {
                // If it's a package, we can index it.
                if(def->getDefinitionType() == Definition::PACKAGE)
                {
                    // Update the scope.
                    PackageDefinition* package = (PackageDefinition*) def;
                    scope = package->getScope();

                    // Try the next key.
                    key = list[i];
                    def = scope->tryGet(key->getValue());
                }
                // Not a package. We can't do anything good.
                else
                {
                    std::ostringstream os;
                    os << "`";
                    scope->printFullyQualifiedName(os, def);
                    os << "` is not a package but was treated as one, when trying to get attribute `" << getName() << "`";
                    error(os.str(), list[i]->getSourcePosition());

                    // Failed.
                    return 0;
                }
            }
            // Doesn't exist. We can't index that.
            else
            {
                std::ostringstream os;
                os << "no package named `";
                if(scope->getPackage())
                {
                    scope->printFullyQualifiedName(os);
                    os << ".";
                }
                os << key->getValue() << "` exists, needed to get attribute `" << getName() << "`";
                error(os.str(), list[i]->getSourcePosition());

                // Failed.
                return 0;
            }
        }
        
        // The definition does not exist.
        if(!def && forbidUndefined)
        {
            std::ostringstream os;
            os << "no attribute named `" << key->getValue() << "` exists in the ";
            if(scope->getPackage())
            {
                os << "package `";
                scope->printFullyQualifiedName(os);
                os << "`";
            }
            else
            {
                os << "current scope";
            }
            os << ", needed to get attribute `" << getName() << "`";
            if(i < list.size())
            {
                error(os.str(), list[i]->getSourcePosition());
            }
            else
            {
                error(os.str(), getSourcePosition());
            }
        }

        // Return the definition we found, or 0 if the definition failed.
        return def;
    }
}