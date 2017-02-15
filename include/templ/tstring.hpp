//
// Created by cc on 1/20/17.
//

#ifndef TEMPL_STRING_HPP
#define TEMPL_STRING_HPP

#include <type_traits>
#include <utility>

#include "basic.hpp"
#include "algorithm.hpp"

namespace templ {

constexpr int const_strcmp(const char *a, const char *b)
{
    unsigned char c1 = '\0',
        c2 = '\0';

    do {
        c1 = static_cast<unsigned char>(*a++);
        c2 = static_cast<unsigned char>(*b++);
        if (c1 == '\0')
            return c1 - c2;
    } while (c1 == c2);

    return c1 - c2;
}

// use octal escape to avoid encoding issues
static_assert(const_strcmp("\1", "\2") < 0, "");
static_assert(const_strcmp("\4", "\3") > 0, "");
static_assert(const_strcmp("\5", "\5") == 0, "");
static_assert(const_strcmp(  "", "\6") < 0, "");
static_assert(const_strcmp("\7",   "") > 0, "");

template<char ...Chars>
struct tstring
{
    static constexpr std::size_t size() {
        return sizeof...(Chars);}

    static std::string as_std_string() {
        return {Chars...};}

    static constexpr std::array<char, size() + 1> as_std_array() {
        return {{Chars..., '\0'}};}

    /// equivalent to strcmp(this, other)
    static constexpr int cmp(const char *other)
    {
        constexpr char arr[] = {Chars..., '\0'};
        return const_strcmp(arr, other);
    }
};

template<
    char ...Cs,
    char ...Rs
>
struct less<templ::tstring<Cs...>, templ::tstring<Rs...>> :
    lexicographical_compare<
        integral_pack<char, Cs...>,
        integral_pack<char, Rs...>
    > {};

template<class S, std::size_t ...I>
tstring<S().str[I]...> make_tstring_impl(std::index_sequence<I...>) {return {};}

template<class S, std::size_t N>
auto make_tstring() {
    return make_tstring_impl<S>(std::make_index_sequence<N>{});
}

/// non-constexpr construction of tstring<> type given a string literal,
/// will be constexpr as of C++17 (constexpr lambdas)
#define MAKE_TSTRING(literal)                                                  \
[]{struct S {const char* str = literal;};                                      \
   return templ::make_tstring<S, sizeof(literal) - 1>();}()

auto test_make_tstring()
{
    return make_pack(
        MAKE_TSTRING(""),
        MAKE_TSTRING("\0"),
        MAKE_TSTRING("hi")
    );
}

static_assert(std::is_same<decltype(test_make_tstring()),
    pack<tstring<>, tstring<'\0'>, tstring<'h', 'i'>>>::value, "");

} // namespace src

#endif // TEMPL_STRING_HPP
