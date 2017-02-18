#ifndef TEMPL_ALGORITHM_HPP
#define TEMPL_ALGORITHM_HPP

#include <utility>
#include <type_traits>
// TODO: see if we can leave this out?
#include <algorithm>

#include "basic.hpp"

namespace templ {

////////////////////////////////////////////////////////////////////////////////
// size, empty, clear                                                         //
////////////////////////////////////////////////////////////////////////////////

template<class T>
struct size_impl
{
    static_assert(false_v<T>,
        "size can only be used on templated class types");
};

template<template<class...> class T, class ...Ts>
struct size_impl<T<Ts...>>
{
    static constexpr std::size_t value = sizeof...(Ts);
};

/// number of elements in T's parameter pack
template<class T>
constexpr std::size_t size_v = size_impl<T>::value;

template<class T>
using size = std::integral_constant<std::size_t, size_v<T>>;




/// is T's parameter pack empty?
template<class T>
constexpr bool empty_v = (size_v<T> == 0);

template<class T>
using empty = bool_constant<empty_v<T>>;



template<class T>
struct clear_impl
{
    static_assert(false_v<T>,
        "clear can only be used on templated class types");
};

template<template<class...> class T, class ...Ts>
struct clear_impl<T<Ts...>>
{
    using type = T<>;
};

/// remove all parameters from T's parameter pack and get the resulting type
template<class T>
using clear_t = typename clear_impl<T>::type;

////////////////////////////////////////////////////////////////////////////////
// push/pop/front                                                             //
////////////////////////////////////////////////////////////////////////////////

template<class T>
struct front_impl
{
    static_assert(false_v<T>, "front_t can only be used on templated class "
        "types with non-empty parameter packs");
};

template<template<class...> class T, class T0, class ...Ts>
struct front_impl<T<T0, Ts...>>
{
    using type = T0;
};

/// get the first type in T's parameter pack
template<class T>
using front_t = typename front_impl<T>::type;



template<class T>
struct pop_front_impl
{
    static_assert(false_v<T>, "pop_front_t can only be used on templated class "
        "types with non-empty parameter packs");
};

template<template<class...> class T, class T0, class ...Ts>
struct pop_front_impl<T<T0, Ts...>>
{
    using type = T<Ts...>;
};

/// remove the first type in T's parameter pack
template<class T>
using pop_front_t = typename pop_front_impl<T>::type;



template<class T, class T0>
struct push_front_impl
{
    static_assert(false_v<T>,
        "push_front_t can only be used on templated class types");
};

template<template<class...> class T, class ...Ts, class T0>
struct push_front_impl<T<Ts...>, T0>
{
    using type = T<T0, Ts...>;
};

/// add the type T0 to the beginning of T's parameter pack
template<class T, class T0>
using push_front_t = typename push_front_impl<T, T0>::type;

////////////////////////////////////////////////////////////////////////////////
// concat                                                                     //
////////////////////////////////////////////////////////////////////////////////

// TODO: note: concat is pretty much flatten for 2 args
template<class T, class U>
struct concat_impl
{
    static_assert(false_v<T>,
        "concat can only be used with two templated class types");
};

template<
    template<class...> class T, class ...Ts,
    template<class...> class U, class ...Us
>
struct concat_impl<T<Ts...>, U<Us...>>
{
    using type = T<Ts..., Us...>;
};

/// append U's parameter pack to T's
template<class T, class U>
using concat_t = typename concat_impl<T, U>::type;

////////////////////////////////////////////////////////////////////////////////
// transform, zip_with, move, flatten                                         //
////////////////////////////////////////////////////////////////////////////////

template<class T, template<class...> class Op>
struct transform_impl;

template<
    template<class...> class T, class ...Ts,
    template<class...> class Op
>
struct transform_impl<T<Ts...>, Op>
{
    using type = T<Op<Ts>...>;
};

/// apply unary Op to each type in T's parameter pack
template<class T, template<class...> class Op>
using transform_t = typename transform_impl<T, Op>::type;



template<class T, class U, template<class...> class Op>
struct zip_with_impl;

template<
    template<class...> class T, class ...Ts,
    template<class...> class U, class ...Us,
    template<class...> class Op
>
struct zip_with_impl<T<Ts...>, U<Us...>, Op>
{
    using type = T<Op<Ts, Us>...>;
};

/// zip parameter packs from T and U into T with binary Op (must be same length)
template<class T, class U, template<class...> class Op>
using zip_with_t = typename zip_with_impl<T, U, Op>::type;



template<class To, class From>
struct move_impl;

template<
    template<class...> class To, class ...ToTypes,
    template<class...> class From, class ...FromTypes
>
struct move_impl<To<ToTypes...>, From<FromTypes...>>
{
    using type = To<FromTypes...>;
};

/// move parameter pack from From into To
template<class To, class From>
using move_t = typename move_impl<To, From>::type;



template<template<class...> class To, class From>
struct construct_impl;

template<
    template<class...> class To,
    template<class...> class From, class ...FromTypes
>
struct construct_impl<To, From<FromTypes...>>
{
    using type = To<FromTypes...>;
};

/// construct T with the template parameters from From
template<template<class...> class T, class From>
using construct_t = typename construct_impl<T, From>::type;



// TODO: note: can prob make this work for T<U<Us...>, U2<U2s...>> in nlog(n)
template<class T>
struct flatten_impl;

template<
    template<class...> class T,
    template<class...> class U, class ...Us
>
struct flatten_impl<T<U<Us...>>>
{
    using type = T<Us...>;
};

/// flatten T's parameter pack, preserving the outermost class
template<class T>
using flatten_t = typename flatten_impl<T>::type;

////////////////////////////////////////////////////////////////////////////////
// fill, fill_n                                                               //
////////////////////////////////////////////////////////////////////////////////

template<class T, class U>
struct fill_impl;

template<class U, template<class...> class T, class ...Ts>
struct fill_impl<T<Ts...>, U>
{
    using type = T<repeat_helper2_t<U, Ts>...>;
};

/// fill T's parameter pack with type U
template<class T, class U>
using fill_t = typename fill_impl<T, U>::type;



template<
    template<class...> class T, std::size_t N, class R,
    class = std::make_index_sequence<N>
>
struct fill_n_impl;

template<
    template<class...> class T,
    std::size_t N, class R, std::size_t ...I
>
struct fill_n_impl<T, N, R, std::index_sequence<I...>>
{
    using type = T<repeat_helper_t<R, I>...>;
};

/// construct T with the type R repeated N times
template<template<class...> class T, std::size_t N, class R>
using fill_n_t = typename fill_n_impl<T, N, R>::type;

////////////////////////////////////////////////////////////////////////////////
// at                                                                         //
////////////////////////////////////////////////////////////////////////////////

// TODO: fix this for clang
//#ifdef __has_builtin(__type_pack_element)
//
//template<class T, std::size_t N>
//struct type_pack_helper;
//
//template<template<class...> class T, class ...Ts, std::size_t N>
//struct type_pack_helper<T<Ts...>, N>
//{
//    using type = __type_pack_element<N, Ts...>;
//};
//
///// get the Nth type from T's parameter pack
//template<class T, std::size_t N>
//using at_t = typename type_pack_helper<T, N>::type;
//
//#else // __has_builtin(__type_pack_element)
//#endif // __has_builtin(__type_pack_element)

// note: essentially the same as tail, but since at_t is used much more often
// it might warrant its own class
template<class T, std::size_t N, class = std::make_index_sequence<N>>
struct at_impl;

template<
    template<class...> class T, class ...Ts,
    std::size_t N, std::size_t ...I
>
struct at_impl<T<Ts...>, N, std::index_sequence<I...>>
{
    template<class U>
    static id<U> get(repeat_helper_t<void*, I>..., id<U>*, ...);

    using type = typename decltype(get( as_ptr_v<id<Ts>>... ))::type;
};

/// get the Nth type from T's parameter pack
template<class T, std::size_t N>
using at_t = typename at_impl<T, N>::type;


////////////////////////////////////////////////////////////////////////////////
// select_ids                                                                 //
////////////////////////////////////////////////////////////////////////////////

// thanks to Louis Dionne, adapted from
// http://ldionne.com/2015/11/29/efficient-parameter-pack-indexing/

template<std::size_t, class>
struct indexed {};

template<class, class...>
struct indexer;

template<std::size_t ...I, class ...Ts>
struct indexer<std::index_sequence<I...>, Ts...>
    : indexed<I, Ts>... {};

template<
    template<class...> class T,
    std::size_t ...I, class ...Ts
>
id<T<Ts...>> select_impl(indexed<I, Ts>...);

template<std::size_t I, class T>
id<T> select_impl_single(indexed<I, T>);

template<class T, class IndexSeq, bool Exec = true>
struct select_helper
{
    using type = T;
};

template<template<class...> class T, class ...Ts, std::size_t ...I>
struct select_helper<T<Ts...>, std::index_sequence<I...>, true>
{
    using input = indexer<std::index_sequence_for<Ts...>, Ts...>;
#ifdef TEMPL_CAN_DEDUCT_SELECT
    using type = typename decltype(select_impl<T, I...>(
        (I, input{})...
    ))::type;
#else
    using type = T<
        typename decltype(select_impl_single<I>(input{}))::type...
    >;
#endif
};

/// get the parameters from T's parameter pack which correspond to the indices
/// contained in the given std::index_sequence IndexSeq
template<class T, class IndexSeq>
using select_ids_t = typename select_helper<T, IndexSeq>::type;

////////////////////////////////////////////////////////////////////////////////
// head, tail, resize                                                         //
////////////////////////////////////////////////////////////////////////////////

/// get the first N parameters from T's parameter pack
template<class T, std::size_t N>
using head_t = typename select_helper<T,
    std::make_index_sequence<N>, (N < size_v<T>)>::type;



template<class T, std::size_t N,
    class = std::make_index_sequence<(size_v<T> < N ? N : size_v<T> - N)>>
struct tail_impl;

template<
    template<class...> class T, class ...Ts,
    std::size_t N, std::size_t ...I
>
struct tail_impl<T<Ts...>, N, std::index_sequence<I...>>
{
    template<class ...Us>
    static id<T<Us...>> get(repeat_helper_t<void*, I>..., id<Us>*...);

    using type = typename decltype(get( as_ptr_v<id<Ts>>... ))::type;
};

/// get the last N parameters from T's parameter pack
template<class T, std::size_t N>
using tail_t = typename tail_impl<T, N>::type;



template<class T, std::size_t N, class U, bool = (N < size_v<T>)>
struct resize_impl;

// we are shortening the parameter pack
template<class T, std::size_t N, class U>
struct resize_impl<T, N, U, true>
{
    using type = head_t<T, N>;
};

// we are adding to the parameter pack
template<class T, std::size_t N, class U>
struct resize_impl<T, N, U, false>
{
    using type = concat_t<T, fill_n_t<pack, (N - size_v<T>), U>>;
};

/// resize parameter pack (if lengthening, pad with third template argument)
template<class T, std::size_t N, class U = void>
using resize_t = typename resize_impl<T, N, U>::type;

////////////////////////////////////////////////////////////////////////////////
// rotate, reverse                                                            //
////////////////////////////////////////////////////////////////////////////////

template<
    class T, std::size_t N = size_v<T>,
    class = std::make_index_sequence<N>
>
struct reverse_impl;

template<class T, std::size_t N, std::size_t ...I>
struct reverse_impl<T, N, std::index_sequence<I...>>
{
    using type = select_ids_t<T, std::index_sequence<(N - I - 1)...>>;
};

/// reverse the types in T's parameter pack
template<class T>
using reverse_t = typename reverse_impl<T>::type;



template<std::size_t, class>
struct rotate_impl;

template<
    std::size_t NFirst,
    template<class...> class T, class ...Ts
>
struct rotate_impl<NFirst, T<Ts...>>
{
    static_assert(NFirst < sizeof...(Ts), "");
    using type = construct_t<T,
        head_t<
            concat_t<
                tail_t<pack<Ts...>, sizeof...(Ts) - NFirst>,
                pack<Ts...>
            >,
            sizeof...(Ts)
        >
    >;
};

/// Rotate T's parameter pack (NFirst becomes the new first element)
template<class T, std::size_t NFirst>
using rotate_t = typename rotate_impl<NFirst, T>::type;

////////////////////////////////////////////////////////////////////////////////
// push/pop/back                                                              //
////////////////////////////////////////////////////////////////////////////////

template<class T, class U>
struct push_back_impl;

template<template<class...> class T, class ...Ts, class U>
struct push_back_impl<T<Ts...>, U>
{
    using type = T<Ts..., U>;
};

template<class T, class U>
using push_back_t = typename push_back_impl<T, U>::type;



template<class T>
using pop_back_t = head_t<T, (size_v<T> == 0 ? 0 : size_v<T> - 1)>;



template<class T>
using back_t = at_t<T, (size_v<T> == 0 ? 0 : size_v<T> - 1)>;

////////////////////////////////////////////////////////////////////////////////
// erase, insert, assign                                                      //
////////////////////////////////////////////////////////////////////////////////

template<std::size_t N, class T, class Head = head_t<construct_t<pack, T>, N>>
struct erase_impl;

template<std::size_t N, template<class...> class T, class ...Ts, class ...Ls>
struct erase_impl<N, T<Ts...>, pack<Ls...>>
{
    template<class OldT, class ...Rs>
    static pack<Ls..., Rs...> erase(pack<Ls..., OldT, Rs...>);

    using type = construct_t<T, decltype(erase(pack<Ts...>{}))>;
};

/// remove type at position N from T's parameter pack
template<class T, std::size_t N>
using erase_t = typename erase_impl<N, T>::type;



template<std::size_t N, class T, class Head = head_t<construct_t<pack, T>, N>>
struct insert_impl;

template<std::size_t N, template<class...> class T, class ...Ts, class ...Ls>
struct insert_impl<N, T<Ts...>, pack<Ls...>>
{
    template<class NewT, class ...Rs>
    static pack<Ls..., NewT, Rs...> insert(pack<Ls..., Rs...>);

    template<class NewT>
    using type = construct_t<T, decltype(insert<NewT>(pack<Ts...>{}))>;
};

/// insert type U before position N in T's parameter pack
template<class T, std::size_t N, class U>
using insert_t = typename insert_impl<N, T>::template type<U>;



template<std::size_t N, class T, class Head = head_t<construct_t<pack, T>, N>>
struct assign_impl;

template<std::size_t N, template<class...> class T, class ...Ts, class ...Ls>
struct assign_impl<N, T<Ts...>, pack<Ls...>>
{
    template<class NewT, class OldT, class ...Rs>
    static pack<Ls..., NewT, Rs...> assign(pack<Ls..., OldT, Rs...>);

    template<class NewT>
    using type = construct_t<T, decltype(assign<NewT>(pack<Ts...>{}))>;
};

/// assign type U to the type currently at position N in T's parameter pack
template<class T, std::size_t N, class U>
using assign_t = typename assign_impl<N, T>::template type<U>;

////////////////////////////////////////////////////////////////////////////////
// predicate_helper                                                           //
////////////////////////////////////////////////////////////////////////////////

template<class T, template<class...> class Predicate = id_t>
struct predicate_helper;

template<
    template<class...> class T, class ...Ts,
    template<class...> class Predicate
>
struct predicate_helper<T<Ts...>, Predicate>
{
    static constexpr std::size_t N = sizeof...(Ts);
    static constexpr bool values[N] = {
        Predicate<Ts>::value...
    };

    static constexpr std::size_t first_true()
    {
        for (std::size_t i = 0; i < N; ++i)
            if (values[i]) {return i;}

        return N;
    }

    static constexpr std::size_t first_false()
    {
        for (std::size_t i = 0; i < N; ++i)
            if (!values[i]) {return i;}

        return N;
    }

    static constexpr std::size_t count()
    {
        std::size_t num = 0;
        for (std::size_t i = 0; i < N; ++i)
            num += values[i];

        return num;
    }

    static constexpr bool all_of()     {return first_false() == N;}
    static constexpr bool any_of()     {return first_true() != N;}
    static constexpr bool none_of()    {return !any_of();}

// TODO: member function order issues? undefined behavior?
    static constexpr auto ids_impl()
    {
        struct {
            std::size_t ids[count()] = {};
            constexpr auto operator[](std::size_t idx) {
                return ids[idx];
            }
        } data;

        for (std::size_t i = 0, id_pos = 0; i < N; ++i)
            if (values[i])
                data.ids[id_pos++] = i;

        return data;
    }

    template<std::size_t ...I>
    static constexpr auto ids_helper(std::index_sequence<I...>)
        -> std::index_sequence<ids_impl()[I]...>;

    static constexpr auto ids()
        -> decltype(ids_helper(std::make_index_sequence<count()>{}));
};

////////////////////////////////////////////////////////////////////////////////
// count, count_if                                                            //
////////////////////////////////////////////////////////////////////////////////

/// how many elements of T's parameter pack satisfy the given predicate
template<class T, template<class...> class Predicate>
constexpr std::size_t count_if_v = predicate_helper<T, Predicate>::count();

template<class T, template<class...> class Predicate>
using count_if = std::integral_constant<std::size_t, count_if_v<T, Predicate>>;




/// how many U's are in T's parameter pack
template<class T, class U>
constexpr std::size_t count_v = count_if_v<T,
    partial<std::is_same, U>::template type>;

template<class T, class U>
using count = std::integral_constant<std::size_t, count_v<T, U>>;

////////////////////////////////////////////////////////////////////////////////
// any_of, all_of, none_of                                                    //
////////////////////////////////////////////////////////////////////////////////

/// whether all types in T's parameter pack satisfy the given unary predicate
template<class T, template<class...> class Predicate>
constexpr bool all_of_v = predicate_helper<T, Predicate>::all_of();

template<class T, template<class...> class Predicate>
using all_of = bool_constant<all_of_v<T, Predicate>>;



/// whether any types in T's parameter pack satisfy the given unary predicate
template<class T, template<class...> class Predicate>
constexpr bool any_of_v = predicate_helper<T, Predicate>::any_of();

template<class T, template<class...> class Predicate>
using any_of = bool_constant<any_of_v<T, Predicate>>;



/// whether no types in T's parameter pack satisfy the given unary predicate
template<class T, template<class...> class Predicate>
constexpr bool none_of_v = predicate_helper<T, Predicate>::none_of();

template<class T, template<class...> class Predicate>
using none_of = bool_constant<none_of_v<T, Predicate>>;

////////////////////////////////////////////////////////////////////////////////
// find, find_if                                                              //
////////////////////////////////////////////////////////////////////////////////

/// get the index of the first element of T's parameter pack that
/// satisfies the given predicate
template<class T, template<class...> class Predicate>
constexpr std::size_t find_if_v = predicate_helper<T, Predicate>::first_true();

template<class T, template<class...> class Predicate>
using find_if = std::integral_constant<std::size_t, find_if_v<T, Predicate>>;




/// get the index of the first element of T's parameter pack that
/// does not satisfy the given predicate
template<class T, template<class...> class Predicate>
constexpr std::size_t find_if_not_v =
    predicate_helper<T, Predicate>::first_false();

template<class T, template<class...> class Predicate>
using find_if_not = std::integral_constant<std::size_t,
    find_if_not_v<T, Predicate>>;



/// get the index of the first U in T's parameter pack
template<class T, class U>
constexpr std::size_t find_v = find_if_v<T,
    partial<std::is_same, U>::template type>;

template<class T, class U>
using find = std::integral_constant<std::size_t, find_v<T, U>>;

////////////////////////////////////////////////////////////////////////////////
// copy_if, remove, remove_if, replace, replace_if                            //
////////////////////////////////////////////////////////////////////////////////

/// append types from the parameter pack of 'From', satisfying the given
/// Predicate, to the parameter pack of 'To'
template<class From, class To, template<class...> class Predicate>
using copy_if_t = concat_t<To,
    select_ids_t<
        construct_t<pack, From>,
        decltype(predicate_helper<From, Predicate>::ids())
    >
>;



/// remove types that satisfy the given Predicate from T's parameter pack
template<class T, template<class...> class Predicate>
using remove_if_t = move_t<T,
    copy_if_t<T, pack<>, compose<negation, Predicate>::template type>>;



/// remove all U's from T's parameter pack
template<class T, class U>
using remove_t = remove_if_t<T, partial<std::is_same, U>::template type>;



template<template<class...> class Predicate, class U>
struct replace_if_helper
{
    // implementation, default case is if the Predicate<T>::value is false
    template<class T, bool = Predicate<T>::value>
    struct replace_if_impl { using type = T; };

    // if the type should be replaced
    template<class T>
    struct replace_if_impl<T, true> { using type = U; };

    template<class T>
    using type = typename replace_if_impl<T>::type;
};

/// replace all elements of T's parameter pack that satisfy Predicate with U's
template<class T, class U, template<class...> class Predicate>
using replace_if_t = transform_t<T,
    replace_if_helper<Predicate, U>::template type>;

/// replace all OldValue's in T's parameter pack with NewValue's
template<class T, class OldValue, class NewValue>
using replace_t = replace_if_t<T, NewValue,
    partial<std::is_same, OldValue>::template type>;

////////////////////////////////////////////////////////////////////////////////
// mismatch, equal                                                            //
////////////////////////////////////////////////////////////////////////////////

template<template<class...> class Predicate, class T, class U>
struct mismatch_helper;

template<
    template<class...> class Predicate,
    template<class...> class T, class ...Ts,
    template<class...> class U, class ...Us
>
struct mismatch_helper<Predicate, T<Ts...>, U<Us...>>
{
    static constexpr std::size_t len = std::min(sizeof...(Ts), sizeof...(Us));
    static constexpr std::size_t value = predicate_helper<
        zip_with_t<
            head_t<pack<Ts...>, len>,
            head_t<pack<Us...>, len>,
            Predicate
        >
    >::first_false();
};

/// get the index of the first elements which fail to satisfy the
/// given binary Predicate
template<class T, class U, template<class...> class Predicate = std::is_same>
constexpr std::size_t mismatch_v = mismatch_helper<Predicate, T, U>::value;

template<class T, class U, template<class...> class Predicate = std::is_same>
using mismatch = std::integral_constant<std::size_t,
    mismatch_v<T, U, Predicate>>;



/// are the two parameter packs for T and U equal?
template<class T, class U>
constexpr std::size_t equal_v = (
    mismatch_v<T, U, std::is_same> == std::max(size_v<T>, size_v<U>));

template<class T, class U>
using equal = bool_constant<equal_v<T, U>>;

////////////////////////////////////////////////////////////////////////////////
// lexicographical_compare                                                    //
////////////////////////////////////////////////////////////////////////////////

template<
    template<class...> class Compare, class T, class U,
    std::size_t Loc =
        mismatch_v<T, U, compose<negation, Compare>::template type>,
    bool BeforeEnd =
        (Loc != std::min(size_v<T>, size_v<U>))
>
struct lexicographical_compare_impl
    : bool_constant<(Loc < size_v<U>)> {};

template<
    template<class...> class Compare,
    class T, class U, std::size_t Loc
>
struct lexicographical_compare_impl<Compare, T, U, Loc, true>
    : bool_constant<Compare<at_t<T, Loc>, at_t<U, Loc>>::value> {};

/// compare two parameter packs lexicographically, using the specified
/// comparator (or templ::less if none provided)
template<class T, class U, template<class...> class Compare = templ::less>
constexpr bool lexicographical_compare_v =
    lexicographical_compare_impl<Compare, T, U>::value;

template<class T, class U, template<class...> class Compare = less>
using lexicographical_compare = bool_constant<
    lexicographical_compare_v<T, U, Compare>>;

////////////////////////////////////////////////////////////////////////////////
// adjacent_helper                                                            //
////////////////////////////////////////////////////////////////////////////////

struct end_marker {};

template<class T, template<class...> class Predicate>
struct adjacent_helper
{
    // if T has an empty parameter pack
    using type = T;
};

template<
    template<class...> class T, class T0, class ...Ts,
    template<class...> class Predicate
>
struct adjacent_helper<T<T0, Ts...>, Predicate>
{
    template<class U, class R>
    struct eval {
        using type = Predicate<U, R>;
    };

    // this triggers for the last element
    template<class U>
    struct eval<U, end_marker> :
        std::false_type {};

    template<class U, class R>
    using eval_t = typename eval<U, R>::type;

    // compare<N, N + 1>
    using type = zip_with_t<
        pack<T0, Ts...>,
        pack<Ts..., end_marker>, eval_t
    >;
};

////////////////////////////////////////////////////////////////////////////////
// unique                                                                     //
////////////////////////////////////////////////////////////////////////////////

// TODO: adjacent_find

// TODO: is_sorted_until, is_sorted

template<class T, template<class...> class Predicate>
struct unique_impl
{
    // triggers if T has an empty parameter pack
    using type = T;
};

template<
    template<class...> class T, class T0, class ...Ts,
    template<class...> class Predicate
>
struct unique_impl<T<T0, Ts...>, Predicate>
{
    template<class U, class R>
    using predicate_not_t = bool_constant<!Predicate<U, R>::value>;

// TODO: need to use pack
    using type = push_front_t<
        select_ids_t<
            // we exclude the first element here
            // due to how unique works with indices
            T<Ts...>,
            decltype(predicate_helper<
                typename adjacent_helper<
                    // need to have all the elements here though
                    T<T0, Ts...>, predicate_not_t
                >::type
            >::ids())
        >, T0 // always add the first element
    >;
};

template<class T, template<class...> class Predicate = std::is_same>
using unique_t = typename unique_impl<T, Predicate>::type;

////////////////////////////////////////////////////////////////////////////////
// partition_point, partition, merge                                          //
////////////////////////////////////////////////////////////////////////////////

/// finds the index of the first occurring false value in the given
/// boolean array, for use with partition_point or similar
template<std::size_t N>
constexpr std::size_t partition_point_search(const bool (&values)[N])
{
    std::size_t L = 0,
        R = N - 1;


    while (L < R)
    {
        const auto midpoint = (L + R) / 2;
        if (values[midpoint])
            L = midpoint + 1;
        else
            R = midpoint;
    }

    return L;
}

template<class T, template<class...> class Predicate>
struct partition_point_impl
{
    // triggers if T's parameter pack is empty
    static constexpr std::size_t value = 0;
};

template<
    template<class...> class T, class T0, class ...Ts,
    template<class...> class Predicate
>
struct partition_point_impl<T<T0, Ts...>, Predicate>
{
    static constexpr bool values[] = {
        Predicate<T0>::value,
        Predicate<Ts>::value...
    };

    static constexpr std::size_t value = partition_point_search(values);
};

/// find the end of the first partition in T's parameter pack, that is,
/// the index of the first element which does not satisfy Predicate
template<class T, template<class...> class Predicate>
using partition_point = partition_point_impl<T, Predicate>;

template<class T, template<class...> class Predicate>
constexpr std::size_t partition_point_v = partition_point<T, Predicate>::value;



template<class T, template<class...> class Compare>
struct partition_impl;

template<
    template<class...> class T, class ...Ts,
    template<class...> class Predicate
>
struct partition_impl<T<Ts...>, Predicate>
{
    using T_left = copy_if_t<pack<Ts...>, pack<>, Predicate>;
    using T_right = copy_if_t<pack<Ts...>, pack<>,
        compose<negation, Predicate>::template type>;

    // partition point index
    static constexpr std::size_t value = size_v<T_left>;

    // concatenate the ranges and construct T with their types
    using type = construct_t<T, concat_t<T_left, T_right>>;
};

/// Partition T's parameter pack according to the given predicate.
/// Relative order of the elements is preserved.
template<class T, template<class...> class Predicate>
using partition_t = typename partition_impl<T, Predicate>::type;



// TODO: conditional_t and front_t errors due to no non-'_t' versions
// base case, triggers if both parameter packs are empty
template<
    class A, class B, template<class...> class Compare,
    bool A_gt_B = (size_v<A> >= size_v<B>)
>
struct merge_impl
{
    using type = A;
};

// base case, triggers if B's parameter pack is empty
template<
    template<class...> class A, class A0, class ...As,
    template<class...> class B,
    template<class...> class Compare
>
struct merge_impl<A<A0, As...>, B<>, Compare, true>
{
    using type = A<A0, As...>;
};

// triggers if B's parameter pack is larger than A's
template<class A, class B, template<class...> class Compare>
struct merge_impl<A, B, Compare, false>
{
    using type = typename merge_impl<B, A, Compare, true>::type;
};

// main implementation, does the actual merging
template<
    template<class...> class A_in, class A0, class ...As,
    template<class...> class B_in, class B0, class ...Bs,
    template<class...> class Compare
>
struct merge_impl<A_in<A0, As...>, B_in<B0, Bs...>, Compare, true>
{
    using A = A_in<A0, As...>;
    using B = B_in<B0, Bs...>;

    static constexpr auto AN = 1 + sizeof...(As);
    static constexpr auto BN = 1 + sizeof...(Bs);

    // split A into two halves (not necessarily equal)
    using A_left = head_t<A, AN / 2>;
    using A_right = tail_t<A, (AN - AN / 2)>;

    // type at A's midpoint value
    using A_midpoint = front_t<A_right>;

    // compare each type in B to our midpoint value
    //      note: not sure how to avoid comparing them all even though they
    //            are sorted
    static constexpr bool values[BN] = {
        Compare<A_midpoint, B0>::value,
        Compare<A_midpoint, Bs>::value...
    };

    // use a binary search to find where to put A_midpoint in B
    static constexpr std::size_t B_midpoint_id = partition_point_search(values);

    // then split B around that point
    using B_left = head_t<B, B_midpoint_id>;
    using B_right = tail_t<B, (BN - B_midpoint_id)>;

    // finally, recursively merge
    using type = concat_t<
        typename merge_impl<A_left, B_left, Compare>::type,
        typename std::conditional_t<
            // triggers if [B_right == B] and [A_right == A], which is an issue
            // because we can't recurse into ourselves the way this is set up
            (B_midpoint_id == 0 && AN == 1),
            concat_impl<A, B>,
            merge_impl<A_right, B_right, Compare>
        >::type
    >;
};

// TODO: probably need to use to_pack_t and whatnot
/// merge sorted B into sorted A using comparator Compare
template<class A, class B, template<class...> class Compare = less>
using merge_t = typename merge_impl<A, B, Compare>::type;

////////////////////////////////////////////////////////////////////////////////
// sort                                                                       //
////////////////////////////////////////////////////////////////////////////////

template<class T, template<class...> class Compare>
struct sort_impl
{
    // triggers if T's parameter pack has less than 2 types in it
    using type = T;
};

template<
    template<class...> class T_in, class T0, class T1, class ...Ts,
    template<class...> class Compare
>
struct sort_impl<T_in<T0, T1, Ts...>, Compare>
{
    static constexpr std::size_t midpoint = (2 + sizeof...(Ts)) / 2;
    using T_midpoint = at_t<T_in<T0, T1, Ts...>, midpoint>;

    // remove the midpoint from T_in
    using T = erase_t<T_in<T0, T1, Ts...>, midpoint>;

    // partition T into elements that compare less than the midpoint value
    template<class U>
    using left_predicate = Compare<U, T_midpoint>;

    using T_left = copy_if_t<T, pack<>, left_predicate>;

    // and into elements that do not
    template<class U>
    using right_predicate = bool_constant<!left_predicate<U>::value>;

    using T_right = copy_if_t<T, pack<>, right_predicate>;

    // recursively call sort on the left and right
    using type = concat_t<
        typename sort_impl<T_left, Compare>::type,
        push_front_t<
            typename sort_impl<T_right, Compare>::type,
            T_midpoint
        >
    >;
};

/// sort T's parameter pack using Compare
template<class T, template<class...> class Compare = less>
using sort_t = move_t<T, typename sort_impl<T, Compare>::type>;

} // namespace templ

#endif // TEMPL_ALGORITHM_HPP
