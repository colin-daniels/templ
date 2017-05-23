#ifndef TEMPL_TEST_COMMON_HPP
#define TEMPL_TEST_COMMON_HPP

#include "templ/basic.hpp"
#include <utility>

namespace templ {
namespace test {

/// helper for declaring integral constants in tests
template<std::size_t N>
using V = std::integral_constant<std::size_t, N>;

/// helper class for simplifying writing tests
template<std::size_t ...Vals>
using integral_pack = pack<std::integral_constant<std::size_t, Vals>...>;

/// test pack for testing template methods
template<class ...Ts>
struct tpack {virtual ~tpack() = default; virtual void f() = 0;};

/// testing class that can only take a specific number of template args
template<class T1, class T2, class T3, class T4>
struct fixed_pack {};

} // namespace test
} // namespace templ

#define TEMPL_ESC(...) __VA_ARGS__

#define TEMPL_ASSERT_SAME(expected, actual) \
static_assert(std::is_same<TEMPL_ESC expected, TEMPL_ESC actual>::value, \
"Expected "#expected" from "#actual);

#define TEMPL_ASSERT_VALUE(expected, actual) \
static_assert(TEMPL_ESC expected == TEMPL_ESC actual ::value, \
"Expected "#expected" from "#actual);


#endif // TEMPL_TEST_COMMON_HPP
