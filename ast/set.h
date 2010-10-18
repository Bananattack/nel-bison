#pragma once

#if defined(NEL_USE_CPP0X)
#include <unordered_set>
#elif defined(NEL_USE_TR1)
#include <tr1/unordered_set>
#elif defined(NEL_USE_MSVC_STDEXT_HASHSET)
#include <hash_set>
#else
#include <set>
#endif

namespace nel
{
    class SymbolTable;
    
    /**
     * A set of keys which doesn't necessarily need to respect order.
     * Switches to a set type which offers better performance (a hash set) if configured.
     * Uses std::set as a fallback.
     *
     * You can't typedef templated types with specifying parameters,
     * and you can't template typedefs, only classes and functions.
     * So this wraps one as a static member in a struct instead.
     *
     * NEL_USE_CPP0X will use C++0x's std::unordered_set<K>
     * NEL_USE_TR1 will use TR1's std::tr1::unordered_set<K>
     * NEL_USE_MSVC_STDEXT_HASHMAP will use stdext::hash_set<K>
     * Otherwise use a std::set<K>.
     */
    template <typename K>
    struct Set
    {
#if defined(NEL_USE_CPP0X)
        typedef std::unordered_set<K> Type;
#elif defined(NEL_USE_TR1)
        typedef std::tr1::unordered_set<K> Type;
#elif defined(NEL_USE_MSVC_STDEXT_HASHSET)
        typedef stdext::hash_set<K> Type;
#else
        typedef std::set<K> Type;
#endif
    };
}