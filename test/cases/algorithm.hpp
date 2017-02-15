#ifndef TEMPL_TEST_ALGORITHM_HPP
#define TEMPL_TEST_ALGORITHM_HPP

#include "templ/algorithm.hpp"

namespace templ {

////////////////////////////////////////////////////////////////////////////////
// misc                                                                       //
////////////////////////////////////////////////////////////////////////////////

template<std::size_t N>
using lit = std::integral_constant<std::size_t, N>;

////////////////////////////////////////////////////////////////////////////////
// size, empty, clear                                                         //
////////////////////////////////////////////////////////////////////////////////

static_assert(size<pack<void, void, int, void>>::value == 4, "");
static_assert(size<pack<void, int, int, int, int*, pack<>, char>
    >::value == 7, "");
static_assert(size<pack<>>::value == 0, "");



static_assert(!empty<pack<void, int, int, int, int*, char, pack<>>>::value, "");
static_assert(empty<pack<>>::value, "");



static_assert(std::is_same<pack<>,
    clear_t<pack<void, void, int, int, int, int*, char, pack<>>> >::value, "");
static_assert(std::is_same<pack<>, clear_t<pack<>> >::value, "");

////////////////////////////////////////////////////////////////////////////////
// push/pop/front                                                             //
////////////////////////////////////////////////////////////////////////////////

static_assert(std::is_same<int[2],
    front_t<pack<int[2], void, char, pack<>, double>> >::value, "");
static_assert(std::is_same<pack<>, front_t<pack<pack<>>> >::value, "");
static_assert(std::is_same<void, front_t<pack<void>> >::value, "");



static_assert(std::is_same<pack<void, char, pack<>, double>,
    pop_front_t<pack<int, void, char, pack<>, double>> >::value, "");
static_assert(std::is_same<pack<>, pop_front_t<pack<pack<>>> >::value, "");



static_assert(std::is_same<pack<char, int, void, char, pack<>, double>,
    push_front_t<pack<int, void, char, pack<>, double>, char>>::value, "");
static_assert(std::is_same<pack<pack<>>,
    push_front_t<pack<>, pack<>> >::value, "");

////////////////////////////////////////////////////////////////////////////////
// push/pop/back                                                              //
////////////////////////////////////////////////////////////////////////////////

static_assert(std::is_same<pack<void>,
    push_back_t<pack<>, void>>::value, "");
static_assert(std::is_same<pack<void, int>,
    push_back_t<pack<void>, int>>::value, "");



static_assert(std::is_same<pack<>,     pop_back_t<pack<void>>>::value, "");
static_assert(std::is_same<pack<void>, pop_back_t<pack<void, int>>>::value, "");



static_assert(std::is_same<void, back_t<pack<void>>>::value, "");
static_assert(std::is_same<int[2],  back_t<pack<void, int[2]>>>::value, "");

////////////////////////////////////////////////////////////////////////////////
// concat                                                                     //
////////////////////////////////////////////////////////////////////////////////

static_assert(std::is_same<pack<char, int, int>,
    concat_t<pack<char>, pack<int, int>>>::value, "");
static_assert(std::is_same<pack<char>,
    concat_t<pack<>, pack<char>>>::value, "");
static_assert(std::is_same<pack<>,
    concat_t<pack<>, pack<>> >::value, "");

////////////////////////////////////////////////////////////////////////////////
// transform, zip_with, move, flatten                                         //
////////////////////////////////////////////////////////////////////////////////

static_assert(std::is_same<pack<>,
    transform_t<pack<>, std::is_void>>::value, "");
static_assert(std::is_same<pack<const int, const char, const double>,
    transform_t<pack<int, char, double>, std::add_const_t>>::value, "");
static_assert(std::is_same<pack<pack<double>>,
    transform_t<pack<double>, pack>>::value, "");



static_assert(std::is_same<pack<>,
    zip_with_t<pack<>, pack<>, std::pair>>::value, "");
static_assert(std::is_same<pack<std::pair<int, char>, std::pair<double, float>>,
    zip_with_t<pack<int, double>, pack<char, float>, std::pair>>::value, "");
static_assert(std::is_same<pack<pack<int, char>, pack<double, float>>,
    zip_with_t<pack<int, double>, pack<char, float>, pack>>::value, "");



static_assert(std::is_same<std::pair<int, char>,
    move_t<pack<int, char>, std::pair<double, double>>>::value, "");
static_assert(std::is_same<pack<float, float>,
    move_t<std::pair<float, float>, pack<int>>>::value, "");
static_assert(std::is_same<pack<>,
    move_t<pack<>, pack<>>>::value, "");
static_assert(std::is_same<pack<int, char>,
    move_t<pack<int, char>, pack<>>>::value, "");



static_assert(std::is_same<std::pair<int, char>,
    construct_t<std::pair, pack<int, char>>>::value, "");
static_assert(std::is_same<pack<float, float>,
    construct_t<pack, std::pair<float, float>>>::value, "");
static_assert(std::is_same<pack<>,
    construct_t<pack, pack<>>>::value, "");
static_assert(std::is_same<pack<int, char>,
    construct_t<pack, pack<int, char>>>::value, "");



static_assert(std::is_same<pack<>, flatten_t<pack<pack<>>>>::value, "");
static_assert(std::is_same<pack<int, char>,
    flatten_t<pack<std::pair<int, char>>>>::value, "");

////////////////////////////////////////////////////////////////////////////////
// at, reverse, head, tail, resize                                            //
////////////////////////////////////////////////////////////////////////////////

using test_pack = pack<int, char, void, double, pack<>>;

static_assert(std::is_same<int,    at_t<test_pack, 0>>::value, "");
static_assert(std::is_same<char,   at_t<test_pack, 1>>::value, "");
static_assert(std::is_same<void,   at_t<test_pack, 2>>::value, "");
static_assert(std::is_same<double, at_t<test_pack, 3>>::value, "");
static_assert(std::is_same<pack<>, at_t<test_pack, 4>>::value, "");



static_assert(std::is_same<pack<>, reverse_t<pack<>>>::value, "");
static_assert(std::is_same<pack<double>, reverse_t<pack<double>>>::value, "");
static_assert(std::is_same<pack<pack<>, double, void, char, int>,
    reverse_t<test_pack>>::value, "");
static_assert(std::is_same<std::pair<int, char>,
    reverse_t<std::pair<char, int>>>::value, "");



static_assert(std::is_same<pack<>,    head_t<test_pack, 0>>::value, "");
static_assert(std::is_same<pack<int>, head_t<test_pack, 1>>::value, "");
static_assert(std::is_same<pack<int, char, void, double>,
    head_t<test_pack, 4>>::value, "");
static_assert(std::is_same<test_pack,
    head_t<test_pack, 5>>::value, "");



static_assert(std::is_same<pack<>,       tail_t<test_pack, 0>>::value, "");
static_assert(std::is_same<pack<pack<>>, tail_t<test_pack, 1>>::value, "");
static_assert(std::is_same<pack<char, void, double, pack<>>,
    tail_t<test_pack, 4>>::value, "");
static_assert(std::is_same<test_pack,
    tail_t<test_pack, 5>>::value, "");



static_assert(std::is_same<pack<>,    resize_t<test_pack, 0>>::value, "");
static_assert(std::is_same<pack<int>, resize_t<test_pack, 1>>::value, "");
static_assert(std::is_same<test_pack, resize_t<test_pack, 5>>::value, "");
static_assert(std::is_same<pack<int, char, void, double, pack<>, void>,
    resize_t<test_pack, 6>>::value, "");
static_assert(std::is_same<pack<int, char, void, double, pack<>, int, int>,
    resize_t<test_pack, 7, int>>::value, "");

static_assert(std::is_same<pack<char, char, char>,
    resize_t<pack<>, 3, char>>::value, "");
static_assert(std::is_same<pack<>,
    resize_t<pack<>, 0>>::value, "");
static_assert(std::is_same<pack<void>,
    resize_t<pack<>, 1>>::value, "");

////////////////////////////////////////////////////////////////////////////////
// erase, insert, assign                                                      //
////////////////////////////////////////////////////////////////////////////////

static_assert(std::is_same<pack<>,
    erase_t<pack<int>, 0>>::value, "");
static_assert(std::is_same<pack<char, void, double, pack<>>,
    erase_t<test_pack, 0>>::value, "");
static_assert(std::is_same<pack<int, void, double, pack<>>,
    erase_t<test_pack, 1>>::value, "");
static_assert(std::is_same<pack<int, char, double, pack<>>,
    erase_t<test_pack, 2>>::value, "");
static_assert(std::is_same<pack<int, char, void, pack<>>,
    erase_t<test_pack, 3>>::value, "");
static_assert(std::is_same<pack<int, char, void, double>,
    erase_t<test_pack, 4>>::value, "");



static_assert(std::is_same<pack<float>,
    insert_t<pack<>,    0, float>>::value, "");
static_assert(std::is_same<pack<float, int, char, void, double, pack<>>,
    insert_t<test_pack, 0, float>>::value, "");
static_assert(std::is_same<pack<int, float, char, void, double, pack<>>,
    insert_t<test_pack, 1, float>>::value, "");
static_assert(std::is_same<pack<int, char, float, void, double, pack<>>,
    insert_t<test_pack, 2, float>>::value, "");
static_assert(std::is_same<pack<int, char, void, float, double, pack<>>,
    insert_t<test_pack, 3, float>>::value, "");
static_assert(std::is_same<pack<int, char, void, double, float, pack<>>,
    insert_t<test_pack, 4, float>>::value, "");
static_assert(std::is_same<pack<int, char, void, double, pack<>, float>,
    insert_t<test_pack, 5, float>>::value, "");



static_assert(std::is_same<pack<float>,
    assign_t<pack<int>, 0, float>>::value, "");
static_assert(std::is_same<pack<float, char, void, double, pack<>>,
    assign_t<test_pack, 0, float>>::value, "");
static_assert(std::is_same<pack<int, float, void, double, pack<>>,
    assign_t<test_pack, 1, float>>::value, "");
static_assert(std::is_same<pack<int, char, float, double, pack<>>,
    assign_t<test_pack, 2, float>>::value, "");
static_assert(std::is_same<pack<int, char, void, float, pack<>>,
    assign_t<test_pack, 3, float>>::value, "");
static_assert(std::is_same<pack<int, char, void, double, float>,
    assign_t<test_pack, 4, float>>::value, "");

////////////////////////////////////////////////////////////////////////////////
// count, count_if                                                            //
////////////////////////////////////////////////////////////////////////////////

static_assert(count_if<pack<>, std::is_object>::value == 0, "");
static_assert(count_if<pack<void, int, void, int, void>,
    std::is_void>::value == 3, "");
static_assert(count_if<pack<void, int, void, int, void>,
    std::is_compound>::value == 0, "");
static_assert(count_if<pack<void, int, void, int, void>,
    std::is_integral>::value == 2, "");



static_assert(count<pack<>, float>::value == 0, "");
static_assert(count<pack<void, int, void, int, void>, void>::value == 3, "");
static_assert(count<pack<void, int, void, int, void>,  int>::value == 2, "");

////////////////////////////////////////////////////////////////////////////////
// any_of, all_of, none_of                                                    //
////////////////////////////////////////////////////////////////////////////////

static_assert( all_of<pack<>, std::is_void>::value, "");
static_assert( all_of<pack<void, void, void>, std::is_void>::value, "");
static_assert(!all_of<pack<void,  int, void>, std::is_void>::value, "");



static_assert(!any_of<pack<>, std::is_void>::value, "");
static_assert( any_of<pack<void, void, void>, std::is_void>::value, "");
static_assert( any_of<pack<void,  int, void>, std::is_void>::value, "");



static_assert( none_of<pack<>, std::is_compound>::value, "");
static_assert( none_of<pack<void, void, void>, std::is_integral>::value, "");
static_assert(!none_of<pack<int,  int, void>, std::is_void>::value, "");

////////////////////////////////////////////////////////////////////////////////
// find, find_if                                                              //
////////////////////////////////////////////////////////////////////////////////

static_assert(find_if<pack<>, std::is_integral>::value == 0, "");
static_assert(find_if<pack<int>, std::is_integral>::value == 0, "");
static_assert(find_if<pack<int, int, void>, std::is_void>::value == 2, "");
static_assert(find_if<pack<int, int, void>, std::is_compound>::value == 3, "");



static_assert(find<pack<>, void>::value == 0, "");
static_assert(find<pack<void>, int>::value == 1, "");
static_assert(find<pack<void>, void>::value == 0, "");
static_assert(find<pack<int, int, char>, int>::value == 0, "");
static_assert(find<pack<int, int, char>, char>::value == 2, "");

////////////////////////////////////////////////////////////////////////////////
// mismatch                                                                   //
////////////////////////////////////////////////////////////////////////////////

static_assert(mismatch<pack<int, char, void>, pack<int, char>>::value == 2, "");
static_assert(mismatch<pack<char>, pack<char>>::value == 1, "");
static_assert(mismatch<pack<>, pack<>>::value == 0, "");

////////////////////////////////////////////////////////////////////////////////
// copy_if, remove, remove_if, replace, replace_if                            //
////////////////////////////////////////////////////////////////////////////////

static_assert(std::is_same<pack<void, void, void>,
    copy_if_t<pack<int, void, char, void, void>, pack<>, std::is_void>
>::value, "");
static_assert(std::is_same<pack<const int>,
    copy_if_t<pack<const int, int>, pack<>, std::is_const>>::value, "");
static_assert(std::is_same<pack<double, const int>,
    copy_if_t<pack<const int, int>, pack<double>, std::is_const>>::value, "");
static_assert(std::is_same<pack<>,
    copy_if_t<pack<void, double>, pack<>, std::is_class>>::value, "");
static_assert(std::is_same<pack<int>,
    copy_if_t<pack<void, double>, pack<int>, std::is_class>>::value, "");
static_assert(std::is_same<pack<int>,
    copy_if_t<pack<>, pack<int>, std::is_integral>>::value, "");


// TODO remove, remove_if tests


static_assert(std::is_same< pack<float, float>,
    replace_if_t<pack<const int, const double>, float, std::is_const>
>::value, "");

static_assert(std::is_same< pack<void, char*, void, const double>,
    replace_t<pack<int, char*, int, const double>, int, void> >::value, "");
static_assert(std::is_same< pack<void*, double[3]>,
    replace_t<pack<int[1], double[3]>, int[1], void*> >::value, "");
static_assert(std::is_same< pack<>, replace_t<pack<>, int, void> >::value, "");
static_assert(std::is_same< pack<float>,
    replace_t<pack<float>, pack<>, pack<>> >::value, "");

////////////////////////////////////////////////////////////////////////////////
// lexicographical_compare                                                    //
////////////////////////////////////////////////////////////////////////////////

// Lexicographical comparison properties from http://en.cppreference.com/
//
// The first mismatching element defines which range is lexicographically less
//      or greater than the other.
static_assert(!lexicographical_compare_v<
    integral_pack<int, 7, 8, 9, 2>, integral_pack<int, 7, 8, 3, 2>>, "");
static_assert(lexicographical_compare_v<
    integral_pack<int, 2, 5, 4, 4>, integral_pack<int, 2, 5, 4, 5>>, "");
static_assert(lexicographical_compare_v<
              integral_pack<int, 3, 7>, integral_pack<int, 4>>, "");
// If one range is a prefix of another, the shorter range is lexicographically
//      less than the other.
static_assert(!lexicographical_compare_v<
              integral_pack<int, 1, 2>, integral_pack<int, 1>>, "");
static_assert(lexicographical_compare_v<
              integral_pack<int, 6>, integral_pack<int, 6, 3>>, "");
// If two ranges have equivalent elements and are of the same length, then the
//      ranges are lexicographically equal.
static_assert(!lexicographical_compare_v<
              integral_pack<int, 5, 2>, integral_pack<int, 5, 2>>, "");
// Two empty ranges are lexicographically equal.
static_assert(!lexicographical_compare_v<pack<>, pack<>>, "");
// An empty range is lexicographically less than any non-empty range.
static_assert(lexicographical_compare_v<pack<>, integral_pack<int, 5>>, "");
static_assert(!lexicographical_compare_v<integral_pack<int, 8, 9>, pack<>>, "");

////////////////////////////////////////////////////////////////////////////////
// unique                                                                     //
////////////////////////////////////////////////////////////////////////////////

static_assert(std::is_same< pack<int, char, double>,
    unique_t<pack<int, int, int, char, double, double>> >::value, "");
static_assert(std::is_same< pack<char, void, double&>,
    unique_t<pack<char, void, double&>> >::value, "");
static_assert(std::is_same< pack<int>,
    unique_t<pack<int, int, int, int, int, int>> >::value, "");
static_assert(std::is_same< pack<>, unique_t<pack<>> >::value, "");

////////////////////////////////////////////////////////////////////////////////
// sort, merge                                                                //
////////////////////////////////////////////////////////////////////////////////

static_assert(less<lit<0>, lit<1>>::value, "");
static_assert(!less<lit<90>, lit<30>>::value, "");
static_assert(!less<lit<30>, lit<30>>::value, "");



static_assert(std::is_same<pack<>, merge_t<pack<>, pack<>>>::value, "");
static_assert(std::is_same<pack<lit<0>, lit<1>, lit<2>, lit<3>>,
    merge_t<pack<lit<0>, lit<3>>, pack<lit<1>, lit<2>>>>::value, "");
static_assert(std::is_same<pack<lit<0>, lit<1>, lit<2>, lit<3>>,
    merge_t<pack<lit<0>, lit<1>, lit<2>, lit<3>>, pack<>>>::value, "");
static_assert(std::is_same<pack<lit<0>, lit<1>, lit<2>, lit<3>>,
    merge_t<pack<>, pack<lit<0>, lit<1>, lit<2>, lit<3>>>>::value, "");



static_assert(std::is_same<pack<>, sort_t<pack<>>>::value, "");
static_assert(std::is_same<pack<lit<0>>, sort_t<pack<lit<0>>>>::value, "");
static_assert(std::is_same<pack<lit<0>, lit<1>, lit<2>, lit<3>>,
    sort_t<pack<lit<3>, lit<2>, lit<1>, lit<0>>>>::value, "");
static_assert(std::is_same<pack<lit<0>, lit<1>, lit<2>, lit<3>>,
    sort_t<pack<lit<0>, lit<2>, lit<1>, lit<3>>>>::value, "");
static_assert(std::is_same<pack<lit<0>, lit<1>, lit<2>, lit<3>>,
    sort_t<pack<lit<3>, lit<1>, lit<2>, lit<0>>>>::value, "");
static_assert(std::is_same<pack<lit<0>, lit<1>, lit<2>, lit<3>>,
    sort_t<pack<lit<1>, lit<0>, lit<3>, lit<2>>>>::value, "");
static_assert(std::is_same<pack<lit<0>, lit<1>, lit<2>, lit<3>>,
    sort_t<pack<lit<2>, lit<0>, lit<3>, lit<1>>>>::value, "");
static_assert(std::is_same<pack<lit<0>, lit<1>, lit<2>, lit<3>>,
    sort_t<pack<lit<2>, lit<3>, lit<0>, lit<1>>>>::value, "");

} // namespace src

#endif // TEMPL_TEST_ALGORITHM_HPP