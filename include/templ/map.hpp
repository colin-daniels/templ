#ifndef TEMPL_MAP_HPP
#define TEMPL_MAP_HPP

#include "algorithm.hpp"

namespace templ {

template<class Key, class ...Values>
struct keyval
{
    using key = Key;

    template<std::size_t N>
    using value_t = templ::at_t<pack<Values...>, N>;
};

/// the map type is meant to be used with templ::keyval as types
template<class ...Ts>
using map = pack<Ts...>;

template<class Key>
struct has_key
{
    template<class KeyValue>
    using type = bool_constant<
        std::is_same<Key, typename KeyValue::key>::value>;
};



template<class T, class Key>
constexpr auto map_find_v = find_if_v<T, has_key<Key>::template type>;

template<class T, class Key>
using map_find = std::integral_constant<std::size_t, map_find_v<T, Key>>;


template<class T, class Key>
using map_at_t = at_t<T, map_find_v<T, Key>>;

////////////////////////////////////////////////////////////////////////////////
// other useful constructs                                                    //
////////////////////////////////////////////////////////////////////////////////

template<class Key, class ...KeyValues>
using switch_t = typename map_at_t<map<KeyValues...>, Key>::template value_t<0>;

} // namespace templ

#endif // TEMPL_MAP_HPP