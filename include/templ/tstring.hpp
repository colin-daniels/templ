#ifndef TEMPL_STRING_HPP
#define TEMPL_STRING_HPP

#include <type_traits>
#include <utility>
#include <string>
#include <array>

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

constexpr std::size_t const_strlen(const char *str)
{
    std::size_t len = 0;
    while (*str++ != '\0')
        ++len;

    return len;
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

    constexpr char operator[](std::size_t idx) const {
        return as_std_array()[idx]; }

    static constexpr char front() { return tstring<Chars...>{}[0]; }
    static constexpr char  back() {
        return tstring<Chars...>{}[size() - (size() > 0)];}

////////////////////////////////////////////////////////////////////////////////
// Comparison operators                                                       //
////////////////////////////////////////////////////////////////////////////////

    /// equivalent to strcmp(this, other)
    static constexpr int cmp(const char *other)
    {
        constexpr char arr[] = {Chars..., '\0'};
        return const_strcmp(arr, other);
    }

    template<char ...Rs>
    friend constexpr bool operator<(const tstring<Chars...>&,
        const tstring<Rs...>&)
    {
        constexpr char ar[] = {Chars..., '\0'}, br[] = {Rs..., '\0'};
        return const_strcmp(ar, br) < 0;
    }

    friend constexpr bool operator<(const tstring<Chars...> &a, const char *b) {
        return a.cmp(b) < 0; }

    friend constexpr bool operator<(const char *a, const tstring<Chars...> &b) {
        return b.cmp(a) > 0; }

////////////////////////////////////////////////////////////////////////////////
// Some string functions                                                      //
////////////////////////////////////////////////////////////////////////////////

    template<std::size_t ...Is>
    static constexpr auto select()
    {
        constexpr char arr[] = {Chars...};
        return tstring<arr[Is]...>{};
    }

    static constexpr auto find_last_of(char c)
    {
        using idx_type = std::string::size_type;
        constexpr char arr[] = {Chars..., '\0'};

        auto last = std::string::npos;
        for (idx_type i = 0; i < sizeof(arr); ++i)
            if (arr[i] == c)
                last = i;

        return last;
    }

    static constexpr std::string::size_type find_first_of(char c)
    {
        using idx_type = std::string::size_type;
        constexpr char arr[] = {Chars..., '\0'};

        for (idx_type i = 0; i < sizeof(arr); ++i)
            if (arr[i] == c)
                return i;

        return std::string::npos;
    }

    template<std::size_t pos, std::size_t ...Is>
    static constexpr auto substr_impl(std::index_sequence<Is...>) {
        return select<(pos + Is)...>();}

    template<std::size_t pos,
        std::size_t count = std::min(std::string::npos, size() - pos)>
    static constexpr auto substr()
    {
        static_assert(pos <= size(), "Substring starting position must be "
            "less than string length.");

        return substr_impl<pos>(std::make_index_sequence<count>{});
    }

////////////////////////////////////////////////////////////////////////////////
// Conversion functions                                                       //
////////////////////////////////////////////////////////////////////////////////

    static std::string as_std_string() {
        return {Chars...};}

    static constexpr std::array<char, size() + 1> as_std_array() {
        return {{Chars..., '\0'}};}
};

template<
    char ...Cs,
    char ...Rs
>
struct less<templ::tstring<Cs...>, templ::tstring<Rs...>> :
    bool_constant<(templ::tstring<Cs...>{} < templ::tstring<Rs...>{})> {};


template<typename S, std::size_t ...Is>
auto make_tstring_impl(std::index_sequence<Is...>)
    -> tstring<S().s[Is]...>;

template<typename S, std::size_t N = const_strlen(S().s)>
auto make_tstring()
    -> decltype(make_tstring_impl<S>(std::make_index_sequence<N>{}))
{
    return {};
}


/// non-constexpr construction of tstring<> type given a string literal,
/// will be constexpr as of C++17 (constexpr lambdas)
#define MAKE_TSTRING(literal)                                                  \
[]{struct S {const char* s = literal;}; return templ::make_tstring<S>();}()

auto test_make_tstring()
{
    return make_pack(
        MAKE_TSTRING(""),
        MAKE_TSTRING("\0"),
        MAKE_TSTRING("hi")
    );
}

static_assert(std::is_same<decltype(test_make_tstring()),
    pack<tstring<>, tstring<>, tstring<'h', 'i'>>>::value, "");

} // namespace templ

#endif // TEMPL_STRING_HPP
