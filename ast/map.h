#pragma once

#if defined(NEL_USE_CPP0X)
#include <unordered_map>
#elif defined(NEL_USE_TR1)
#include <tr1/unordered_map>
#elif defined(NEL_USE_MSVC_STDEXT_HASHMAP)
#include <hash_map>
#else
#include <map>
#endif

namespace nel
{
    class SymbolTable;
    
    /**
     * A map of keys to values which doesn't necessarily need to respect order.
     * Switches to a mapping type which offers better performance (a hashmap) if configured.
     * Uses std::map as a fallback.
     *
     * You can't typedef templated types with specifying parameters,
     * and you can't template typedefs, only classes and functions.
     * So this wraps one as a static member in a struct instead.
     *
     * NEL_USE_CPP0X will use C++0x's std::unordered_map<K, V>
     * NEL_USE_TR1 will use TR1's std::tr1::unordered_map<K, V>
     * NEL_USE_MSVC_STDEXT_HASHMAP will use stdext::hash_map<K, V>
     * Otherwise use a std::map<K, V>.
     */
    template <typename K, typename V>
    struct Map
    {
#if defined(NEL_USE_CPP0X)
        typedef std::unordered_map<K, V> Type;
#elif defined(NEL_USE_TR1)
        typedef std::tr1::unordered_map<K, V> Type;
#elif defined(NEL_USE_MSVC_STDEXT_HASHMAP)
        typedef stdext::hash_map<K, V> Type;
#else
        typedef std::map<K, V> Type;
#endif
    };
}