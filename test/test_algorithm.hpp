#ifndef TEMPL_TEST_ALGORITHM_HPP
#define TEMPL_TEST_ALGORITHM_HPP

#include "test_common.hpp"
#include "templ/algorithm.hpp"

namespace templ {
namespace test {

////////////////////////////////////////////////////////////////////////////////
// size, empty, clear                                                         //
////////////////////////////////////////////////////////////////////////////////

TEMPL_ASSERT_VALUE((0), (size<tpack<>>))
TEMPL_ASSERT_VALUE((4), (size<fixed_pack<int, char, void, int[2]>>))
TEMPL_ASSERT_VALUE((7), (size<tpack<void, int, int, int, int*, tpack<>, char>>))



static_assert(!empty<tpack<void, int, int, int, int*, char, tpack<>>>::value, "");
static_assert(empty<tpack<>>::value, "");



static_assert(std::is_same<tpack<>,
    clear_t<tpack<void, void, int, int, int, int*, char, tpack<>>> >::value, "");
static_assert(std::is_same<tpack<>, clear_t<tpack<>> >::value, "");

////////////////////////////////////////////////////////////////////////////////
// push/pop/front                                                             //
////////////////////////////////////////////////////////////////////////////////

TEMPL_ASSERT_SAME( (float), (front_t<tpack<float, char>>))
TEMPL_ASSERT_SAME(  (void), (front_t<tpack<void>>))
TEMPL_ASSERT_SAME(  (int&), (front_t<fixed_pack<int&, char, void, tpack<>>>))
TEMPL_ASSERT_SAME((int[2]), (front_t<tpack<int[2], char&, tpack<>>>))

static_assert(std::is_same<tpack<void, char, tpack<>, double>,
    pop_front_t<tpack<int, void, char, tpack<>, double>> >::value, "");
static_assert(std::is_same<tpack<>, pop_front_t<tpack<tpack<>>> >::value, "");



static_assert(std::is_same<tpack<char, int, void, char, tpack<>, double>,
    push_front_t<tpack<int, void, char, tpack<>, double>, char>>::value, "");
static_assert(std::is_same<tpack<tpack<>>,
    push_front_t<tpack<>, tpack<>> >::value, "");

////////////////////////////////////////////////////////////////////////////////
// push/pop/back                                                              //
////////////////////////////////////////////////////////////////////////////////

static_assert(std::is_same<tpack<void>,
    push_back_t<tpack<>, void>>::value, "");
static_assert(std::is_same<tpack<void, int>,
    push_back_t<tpack<void>, int>>::value, "");



static_assert(std::is_same<tpack<>,     pop_back_t<tpack<void>>>::value, "");
static_assert(std::is_same<tpack<void>, pop_back_t<tpack<void, int>>>::value, "");



static_assert(std::is_same<void, back_t<tpack<void>>>::value, "");
static_assert(std::is_same<int[2],  back_t<tpack<void, int[2]>>>::value, "");

////////////////////////////////////////////////////////////////////////////////
// concat                                                                     //
////////////////////////////////////////////////////////////////////////////////

static_assert(std::is_same<tpack<char, int, int>,
    concat_t<tpack<char>, tpack<int, int>>>::value, "");
static_assert(std::is_same<tpack<char>,
    concat_t<tpack<>, tpack<char>>>::value, "");
static_assert(std::is_same<tpack<>,
    concat_t<tpack<>, tpack<>> >::value, "");

////////////////////////////////////////////////////////////////////////////////
// transform, zip_with, move, flatten                                         //
////////////////////////////////////////////////////////////////////////////////

static_assert(std::is_same<tpack<>,
    transform_t<tpack<>, std::is_void>>::value, "");
static_assert(std::is_same<tpack<const int, const char, const double>,
    transform_t<tpack<int, char, double>, std::add_const_t>>::value, "");
static_assert(std::is_same<tpack<tpack<double>>,
    transform_t<tpack<double>, tpack>>::value, "");



static_assert(std::is_same<tpack<>,
    zip_with_t<tpack<>, tpack<>, std::pair>>::value, "");
static_assert(std::is_same<tpack<std::pair<int, char>, std::pair<double, float>>,
    zip_with_t<tpack<int, double>, tpack<char, float>, std::pair>>::value, "");
static_assert(std::is_same<tpack<tpack<int, char>, tpack<double, float>>,
    zip_with_t<tpack<int, double>, tpack<char, float>, tpack>>::value, "");



static_assert(std::is_same<std::pair<int, char>,
    move_t<std::pair<double, double>, tpack<int, char>>>::value, "");
static_assert(std::is_same<tpack<float, float>,
    move_t<tpack<int>, std::pair<float, float>>>::value, "");
static_assert(std::is_same<tpack<>,
    move_t<tpack<>, tpack<>>>::value, "");
static_assert(std::is_same<tpack<int, char>,
    move_t<tpack<>, tpack<int, char>>>::value, "");



static_assert(std::is_same<std::pair<int, char>,
    construct_t<std::pair, tpack<int, char>>>::value, "");
static_assert(std::is_same<tpack<float, float>,
    construct_t<tpack, std::pair<float, float>>>::value, "");
static_assert(std::is_same<tpack<>,
    construct_t<tpack, tpack<>>>::value, "");
static_assert(std::is_same<tpack<int, char>,
    construct_t<tpack, tpack<int, char>>>::value, "");



static_assert(std::is_same<tpack<>, flatten_t<tpack<tpack<>>>>::value, "");
static_assert(std::is_same<tpack<int, char>,
    flatten_t<tpack<std::pair<int, char>>>>::value, "");

////////////////////////////////////////////////////////////////////////////////
// at, reverse, head, tail, resize                                            //
////////////////////////////////////////////////////////////////////////////////

using test_pack = tpack<int, char, void, double, tpack<>>;

static_assert(std::is_same<int,    at_t<test_pack, 0>>::value, "");
static_assert(std::is_same<char,   at_t<test_pack, 1>>::value, "");
static_assert(std::is_same<void,   at_t<test_pack, 2>>::value, "");
static_assert(std::is_same<double, at_t<test_pack, 3>>::value, "");
static_assert(std::is_same<tpack<>, at_t<test_pack, 4>>::value, "");



static_assert(std::is_same<tpack<>, reverse_t<tpack<>>>::value, "");
static_assert(std::is_same<tpack<double>, reverse_t<tpack<double>>>::value, "");
static_assert(std::is_same<tpack<tpack<>, double, void, char, int>,
    reverse_t<test_pack>>::value, "");
static_assert(std::is_same<std::pair<int, char>,
    reverse_t<std::pair<char, int>>>::value, "");



static_assert(std::is_same<tpack<>,    head_t<test_pack, 0>>::value, "");
static_assert(std::is_same<tpack<int>, head_t<test_pack, 1>>::value, "");
static_assert(std::is_same<tpack<int, char, void, double>,
    head_t<test_pack, 4>>::value, "");
static_assert(std::is_same<test_pack,
    head_t<test_pack, 5>>::value, "");



static_assert(std::is_same<tpack<>,       tail_t<test_pack, 0>>::value, "");
static_assert(std::is_same<tpack<tpack<>>, tail_t<test_pack, 1>>::value, "");
static_assert(std::is_same<tpack<char, void, double, tpack<>>,
    tail_t<test_pack, 4>>::value, "");
static_assert(std::is_same<test_pack,
    tail_t<test_pack, 5>>::value, "");



static_assert(std::is_same<tpack<>,    resize_t<test_pack, 0>>::value, "");
static_assert(std::is_same<tpack<int>, resize_t<test_pack, 1>>::value, "");
static_assert(std::is_same<test_pack, resize_t<test_pack, 5>>::value, "");
static_assert(std::is_same<tpack<int, char, void, double, tpack<>, void>,
    resize_t<test_pack, 6>>::value, "");
static_assert(std::is_same<tpack<int, char, void, double, tpack<>, int, int>,
    resize_t<test_pack, 7, int>>::value, "");

static_assert(std::is_same<tpack<char, char, char>,
    resize_t<tpack<>, 3, char>>::value, "");
static_assert(std::is_same<tpack<>,
    resize_t<tpack<>, 0>>::value, "");
static_assert(std::is_same<tpack<void>,
    resize_t<tpack<>, 1>>::value, "");

////////////////////////////////////////////////////////////////////////////////
// erase, insert, assign                                                      //
////////////////////////////////////////////////////////////////////////////////

static_assert(std::is_same<tpack<>,
    erase_t<tpack<int>, 0>>::value, "");
static_assert(std::is_same<tpack<char, void, double, tpack<>>,
    erase_t<test_pack, 0>>::value, "");
static_assert(std::is_same<tpack<int, void, double, tpack<>>,
    erase_t<test_pack, 1>>::value, "");
static_assert(std::is_same<tpack<int, char, double, tpack<>>,
    erase_t<test_pack, 2>>::value, "");
static_assert(std::is_same<tpack<int, char, void, tpack<>>,
    erase_t<test_pack, 3>>::value, "");
static_assert(std::is_same<tpack<int, char, void, double>,
    erase_t<test_pack, 4>>::value, "");



static_assert(std::is_same<tpack<float>,
    insert_t<tpack<>,    0, float>>::value, "");
static_assert(std::is_same<tpack<float, int, char, void, double, tpack<>>,
    insert_t<test_pack, 0, float>>::value, "");
static_assert(std::is_same<tpack<int, float, char, void, double, tpack<>>,
    insert_t<test_pack, 1, float>>::value, "");
static_assert(std::is_same<tpack<int, char, float, void, double, tpack<>>,
    insert_t<test_pack, 2, float>>::value, "");
static_assert(std::is_same<tpack<int, char, void, float, double, tpack<>>,
    insert_t<test_pack, 3, float>>::value, "");
static_assert(std::is_same<tpack<int, char, void, double, float, tpack<>>,
    insert_t<test_pack, 4, float>>::value, "");
static_assert(std::is_same<tpack<int, char, void, double, tpack<>, float>,
    insert_t<test_pack, 5, float>>::value, "");



static_assert(std::is_same<tpack<float>,
    assign_t<tpack<int>, 0, float>>::value, "");
static_assert(std::is_same<tpack<float, char, void, double, tpack<>>,
    assign_t<test_pack, 0, float>>::value, "");
static_assert(std::is_same<tpack<int, float, void, double, tpack<>>,
    assign_t<test_pack, 1, float>>::value, "");
static_assert(std::is_same<tpack<int, char, float, double, tpack<>>,
    assign_t<test_pack, 2, float>>::value, "");
static_assert(std::is_same<tpack<int, char, void, float, tpack<>>,
    assign_t<test_pack, 3, float>>::value, "");
static_assert(std::is_same<tpack<int, char, void, double, float>,
    assign_t<test_pack, 4, float>>::value, "");

////////////////////////////////////////////////////////////////////////////////
// count, count_if                                                            //
////////////////////////////////////////////////////////////////////////////////

static_assert(count_if<tpack<>, std::is_object>::value == 0, "");
static_assert(count_if<tpack<void, int, void, int, void>,
    std::is_void>::value == 3, "");
static_assert(count_if<tpack<void, int, void, int, void>,
    std::is_compound>::value == 0, "");
static_assert(count_if<tpack<void, int, void, int, void>,
    std::is_integral>::value == 2, "");



static_assert(count<tpack<>, float>::value == 0, "");
static_assert(count<tpack<void, int, void, int, void>, void>::value == 3, "");
static_assert(count<tpack<void, int, void, int, void>,  int>::value == 2, "");

////////////////////////////////////////////////////////////////////////////////
// any_of, all_of, none_of                                                    //
////////////////////////////////////////////////////////////////////////////////

static_assert( all_of<tpack<>, std::is_void>::value, "");
static_assert( all_of<tpack<void, void, void>, std::is_void>::value, "");
static_assert(!all_of<tpack<void,  int, void>, std::is_void>::value, "");



static_assert(!any_of<tpack<>, std::is_void>::value, "");
static_assert( any_of<tpack<void, void, void>, std::is_void>::value, "");
static_assert( any_of<tpack<void,  int, void>, std::is_void>::value, "");



static_assert( none_of<tpack<>, std::is_compound>::value, "");
static_assert( none_of<tpack<void, void, void>, std::is_integral>::value, "");
static_assert(!none_of<tpack<int,  int, void>, std::is_void>::value, "");

////////////////////////////////////////////////////////////////////////////////
// find, find_if                                                              //
////////////////////////////////////////////////////////////////////////////////

static_assert(find_if<tpack<>, std::is_integral>::value == 0, "");
static_assert(find_if<tpack<int>, std::is_integral>::value == 0, "");
static_assert(find_if<tpack<int, int, void>, std::is_void>::value == 2, "");
static_assert(find_if<tpack<int, int, void>, std::is_compound>::value == 3, "");



static_assert(find<tpack<>, void>::value == 0, "");
static_assert(find<tpack<void>, int>::value == 1, "");
static_assert(find<tpack<void>, void>::value == 0, "");
static_assert(find<tpack<int, int, char>, int>::value == 0, "");
static_assert(find<tpack<int, int, char>, char>::value == 2, "");

////////////////////////////////////////////////////////////////////////////////
// mismatch                                                                   //
////////////////////////////////////////////////////////////////////////////////

static_assert(mismatch<tpack<int, char, void>, tpack<int, char>>::value == 2, "");
static_assert(mismatch<tpack<char>, tpack<char>>::value == 1, "");
static_assert(mismatch<tpack<>, tpack<>>::value == 0, "");

////////////////////////////////////////////////////////////////////////////////
// copy_if, remove, remove_if, replace, replace_if                            //
////////////////////////////////////////////////////////////////////////////////

static_assert(std::is_same<tpack<void, void, void>,
    copy_if_t<tpack<int, void, char, void, void>, tpack<>, std::is_void>
>::value, "");
static_assert(std::is_same<tpack<const int>,
    copy_if_t<tpack<const int, int>, tpack<>, std::is_const>>::value, "");
static_assert(std::is_same<tpack<double, const int>,
    copy_if_t<tpack<const int, int>, tpack<double>, std::is_const>>::value, "");
static_assert(std::is_same<tpack<>,
    copy_if_t<tpack<void, double>, tpack<>, std::is_class>>::value, "");
static_assert(std::is_same<tpack<int>,
    copy_if_t<tpack<void, double>, tpack<int>, std::is_class>>::value, "");
static_assert(std::is_same<tpack<int>,
    copy_if_t<tpack<>, tpack<int>, std::is_integral>>::value, "");


// TODO remove, remove_if tests


static_assert(std::is_same< tpack<float, float>,
    replace_if_t<tpack<const int, const double>, float, std::is_const>
>::value, "");

static_assert(std::is_same< tpack<void, char*, void, const double>,
    replace_t<tpack<int, char*, int, const double>, int, void> >::value, "");
static_assert(std::is_same< tpack<void*, double[3]>,
    replace_t<tpack<int[1], double[3]>, int[1], void*> >::value, "");
static_assert(std::is_same< tpack<>, replace_t<tpack<>, int, void> >::value, "");
static_assert(std::is_same< tpack<float>,
    replace_t<tpack<float>, tpack<>, tpack<>> >::value, "");

////////////////////////////////////////////////////////////////////////////////
// lexicographical_compare                                                    //
////////////////////////////////////////////////////////////////////////////////

// Lexicographical comparison properties from http://en.cppreference.com/
//
// The first mismatching element defines which range is lexicographically less
//      or greater than the other.
static_assert(!lexicographical_compare_v<
    integral_pack<7, 8, 9, 2>, integral_pack<7, 8, 3, 2>>, "");
static_assert(lexicographical_compare_v<
    integral_pack<2, 5, 4, 4>, integral_pack<2, 5, 4, 5>>, "");
static_assert(lexicographical_compare_v<
              integral_pack<3, 7>, integral_pack<4>>, "");
// If one range is a prefix of another, the shorter range is lexicographically
//      less than the other.
static_assert(!lexicographical_compare_v<
              integral_pack<1, 2>, integral_pack<1>>, "");
static_assert(lexicographical_compare_v<
              integral_pack<6>, integral_pack<6, 3>>, "");
// If two ranges have equivalent elements and are of the same length, then the
//      ranges are lexicographically equal.
static_assert(!lexicographical_compare_v<
              integral_pack<5, 2>, integral_pack<5, 2>>, "");
// Two empty ranges are lexicographically equal.
static_assert(!lexicographical_compare_v<tpack<>, tpack<>>, "");
// An empty range is lexicographically less than any non-empty range.
static_assert(lexicographical_compare_v<tpack<>, integral_pack<5>>, "");
static_assert(!lexicographical_compare_v<integral_pack<8, 9>, tpack<>>, "");

////////////////////////////////////////////////////////////////////////////////
// unique                                                                     //
////////////////////////////////////////////////////////////////////////////////

static_assert(std::is_same< tpack<int, char, double>,
    unique_t<tpack<int, int, int, char, double, double>> >::value, "");
static_assert(std::is_same< tpack<char, void, double&>,
    unique_t<tpack<char, void, double&>> >::value, "");
static_assert(std::is_same< tpack<int>,
    unique_t<tpack<int, int, int, int, int, int>> >::value, "");
static_assert(std::is_same< tpack<>, unique_t<tpack<>> >::value, "");

////////////////////////////////////////////////////////////////////////////////
// sort, merge                                                                //
////////////////////////////////////////////////////////////////////////////////

static_assert(less<V<0>, V<1>>::value, "");
static_assert(!less<V<90>, V<30>>::value, "");
static_assert(!less<V<30>, V<30>>::value, "");



static_assert(std::is_same<tpack<>, merge_t<tpack<>, tpack<>>>::value, "");
static_assert(std::is_same<tpack<V<0>, V<1>, V<2>, V<3>>,
    merge_t<tpack<V<0>, V<3>>, tpack<V<1>, V<2>>>>::value, "");
static_assert(std::is_same<tpack<V<0>, V<1>, V<2>, V<3>>,
    merge_t<tpack<V<0>, V<1>, V<2>, V<3>>, tpack<>>>::value, "");
static_assert(std::is_same<tpack<V<0>, V<1>, V<2>, V<3>>,
    merge_t<tpack<>, tpack<V<0>, V<1>, V<2>, V<3>>>>::value, "");



static_assert(std::is_same<tpack<>, sort_t<tpack<>>>::value, "");
static_assert(std::is_same<tpack<V<0>>, sort_t<tpack<V<0>>>>::value, "");
static_assert(std::is_same<tpack<V<0>, V<1>, V<2>, V<3>>,
    sort_t<tpack<V<3>, V<2>, V<1>, V<0>>>>::value, "");
static_assert(std::is_same<tpack<V<0>, V<1>, V<2>, V<3>>,
    sort_t<tpack<V<0>, V<2>, V<1>, V<3>>>>::value, "");
static_assert(std::is_same<tpack<V<0>, V<1>, V<2>, V<3>>,
    sort_t<tpack<V<3>, V<1>, V<2>, V<0>>>>::value, "");
static_assert(std::is_same<tpack<V<0>, V<1>, V<2>, V<3>>,
    sort_t<tpack<V<1>, V<0>, V<3>, V<2>>>>::value, "");
static_assert(std::is_same<tpack<V<0>, V<1>, V<2>, V<3>>,
    sort_t<tpack<V<2>, V<0>, V<3>, V<1>>>>::value, "");
static_assert(std::is_same<tpack<V<0>, V<1>, V<2>, V<3>>,
    sort_t<tpack<V<2>, V<3>, V<0>, V<1>>>>::value, "");

} // namespace test
} // namespace templ

#endif // TEMPL_TEST_ALGORITHM_HPP