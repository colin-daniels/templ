#ifndef TEMPL_BASIC_HPP
#define TEMPL_BASIC_HPP

#include <type_traits>
#include <utility>

namespace templ {

////////////////////////////////////////////////////////////////////////////////
// Basic utility types                                                        //
////////////////////////////////////////////////////////////////////////////////

/// not implemented, used to trigger compiler errors for template debugging
template<class...>
struct ni;

template<class T, T...>
struct niv;

template<class ...Ts>
ni<Ts...> nifn(Ts&&...) {}



/// convenience 'identity' class to reduce typing
template<class T>
struct id {using type = T;};
// e.g. in most cases (as far as template meta-programming goes)
//
// struct thing {using type = float;}
//     is equivalent to
// struct thing : id<float> {};

template<class T>
using id_t = T;



/// basic 'type container' class used in most templ algorithms
template<class...>
struct pack {};

template<class ...Ts>
constexpr pack<Ts...> make_pack(Ts&&...) {return {};}

template<class T>
constexpr auto as_ptr_v = static_cast<T*>(nullptr);

/// std::size_t integral constant
template<std::size_t N>
using ic_size_t = std::integral_constant<std::size_t, N>;

template<class T>
using t_type = typename T::type;

// TODO: really need to fix pack expansion for partial and compose?

/// partial application of template args
template<template<class...> class Op, class ...Ts>
struct partial
{
    template<class ...Us>
    using type = Op<Ts..., Us...>;
};
// e.g.
// partial<std::is_same, int>::template type
//     is equivalent to
// template<class T>
// using whatever = std::is_same<int, T>



/// compose two template classes
template<
    template<class...> class G,
    template<class...> class F
>
struct compose
{
    template<class ...Ts>
    using type = G<F<Ts...>>;
};
// e.g.:
// compose<negation, std::is_class>::template type
// can be used to tell if a type is NOT a class

////////////////////////////////////////////////////////////////////////////////
// misc utilities for identifying types                                       //
////////////////////////////////////////////////////////////////////////////////

/// get class template from type
template<class>
struct class_template;
// e.g.
// class_template<
//     templ::pack<int, void>
// >::template type<char, float>
//     is equivalent to
// templ::pack<char, float>

template<template<class...> class T, class ...Ts>
struct class_template<T<Ts...>>
{
    template<class ...Us>
    using type = T<Us...>;
};



template<class>
struct member_ptr_value_impl;

template<class T, class U>
struct member_ptr_value_impl<T U::*> : id<T> {};

/// helper template to extract value type out of a member object pointer
template<class T>
using member_ptr_value_t = typename member_ptr_value_impl<T>::type;
// e.g. struct foo {double val;}
// member_ptr_value_t<decltype(&foo::val)> is equivalent to double



template<class>
struct member_ptr_class_impl;

template<class T, class U>
struct member_ptr_class_impl<T U::*> : id<U> {};

/// helper template to extract class type out of a member object pointer
template<class T>
using member_ptr_class_t = typename member_ptr_class_impl<T>::type;
// e.g. struct foo {double val;}
// member_ptr_class_t<decltype(&foo::val)> is equivalent to foo

////////////////////////////////////////////////////////////////////////////////
// Utility types present in (future) standards                                //
// Credit to http://en.cppreference.com/ for implementations                  //
////////////////////////////////////////////////////////////////////////////////

template <bool B>
using bool_constant = std::integral_constant<bool, B>;


template<typename... Ts>
struct make_void {typedef void type;};
template<typename... Ts>
using void_t = typename make_void<Ts...>::type;
// example from cppreference.com
//
// // primary template handles types that do not support pre-increment:
// template< class, class = std::void_t<> >
// struct has_pre_increment_member : std::false_type { };
//
// // specialization recognizes types that do support pre-increment:
// template< class T >
// struct has_pre_increment_member<T,
//     std::void_t<decltype( ++std::declval<T&>() )>
// > : std::true_type { };
//

template<class...> struct conjunction : std::true_type {};
template<class B1> struct conjunction<B1> : B1 {};
template<class B1, class... Bn>
struct conjunction<B1, Bn...>
    : std::conditional_t<bool(B1::value), conjunction<Bn...>, B1> {};
template<class... B>
constexpr bool conjunction_v = conjunction<B...>::value;


template<class...> struct disjunction : std::false_type {};
template<class B1> struct disjunction<B1> : B1 {};
template<class B1, class... Bn>
struct disjunction<B1, Bn...>
    : std::conditional_t<bool(B1::value), B1, disjunction<Bn...>>  {};
template<class... B>
constexpr bool disjunction_v = disjunction<B...>::value;


template<class B>
struct negation : bool_constant<!B::value> {};
template<class B>
constexpr bool negation_v = negation<B>::value;


/// basic is_callable implementation, _not_ from cppreference.com
template<class, class = void_t<>>
struct is_callable_impl : std::false_type {};

template<
    template<class...> class T,
    class F, class ...Args
>
struct is_callable_impl<T<F, Args...>, void_t<std::result_of_t<F(Args...)>>> :
    std::true_type {};

template<class F, class ...Args>
using is_callable = typename is_callable_impl<pack<F, Args...>>::type;

template<class F, class ...Args>
constexpr bool is_callable_v = is_callable<F, Args...>::value;

////////////////////////////////////////////////////////////////////////////////
// less                                                                       //
////////////////////////////////////////////////////////////////////////////////

/// default template for comparisons of template types
template<class, class>
struct less;

template<class T, class U>
constexpr bool less_v = less<T, U>::value;

// TODO: might need to change
/// less than comparison for std::integral_constant types
template<
    class A, A valA,
    class B, A valB
>
struct less<
    std::integral_constant<A, valA>,
    std::integral_constant<B, valB>
> : bool_constant<(valA < valB)> {};

////////////////////////////////////////////////////////////////////////////////
// random things                                                              //
////////////////////////////////////////////////////////////////////////////////

/// make value dependent on type insofar as template instantiation goes
template<class ..., class U>
constexpr auto make_dependent(U&& value) noexcept
{
    return std::forward<U>(value);
}
// e.g.
// template<class T>
// struct foo { static_assert(make_dependent<T>(false), "description"); };
//
// will not throw an error unless a non-specialized foo is instantiated

template<class ...Ts>
constexpr auto false_v = make_dependent<Ts...>(false);


template<class T>
struct is_integral_constant : std::false_type {};

template<class T, T value>
struct is_integral_constant<std::integral_constant<T, value>>
    : std::true_type {};

template<class T>
constexpr bool is_integral_constant_v = is_integral_constant<T>::value;


/// helper alias for repeating types via sequences
template<class T, std::size_t>
using repeat_helper_t = T;


template<class T, class ...>
struct crush_types { using type = T; };

/// helper alias for repeating types via type parameter packs
template<class T, class ...Ts>
using crush_types_t = typename crush_types<T, Ts...>::type;



template<typename T, typename = bool_constant<true>>
struct cexpr_callable_ic : std::false_type {};

template<typename T>
struct cexpr_callable_ic<T, bool_constant<(static_cast<void>(T::value()), true)>>
    : std::true_type {};

template<typename T, typename = bool_constant<true>>
struct cexpr_callable_type : std::false_type {};

template<typename T>
struct cexpr_callable_type<T, bool_constant<(static_cast<void>(T{}()), true)>>
    : std::true_type {};



template<typename T>
struct is_constexpr_callable : cexpr_callable_type<T>::type {};

template<typename F, F ptr>
struct is_constexpr_callable<std::integral_constant<F, ptr>>
    : cexpr_callable_ic<std::integral_constant<F, ptr>>::type {};

template<class T>
constexpr bool is_constexpr_callable_v = is_constexpr_callable<T>::value;

template<class F, F ptr>
constexpr bool is_constexpr_callable_v2 = is_constexpr_callable_v<
    std::integral_constant<F, ptr>>;


////////////////////////////////////////////////////////////////////////////////
// type manipulation                                                          //
////////////////////////////////////////////////////////////////////////////////

/// copy const specifier from U to T
template<class T, class U>
using copy_const_t = std::conditional_t<
    std::is_const<U>::value,
    std::add_const_t<T>, T
>;

/// copy volatile specifier from U to T
template<class T, class U>
using copy_volatile_t = std::conditional_t<
    std::is_volatile<U>::value,
    std::add_volatile_t<T>, T
>;

/// copy cv specifiers from U to T
template<class T, class U>
using copy_cv_t = copy_const_t<copy_volatile_t<T, U>, U>;



/// copy lvalue reference from U to T
template<class T, class U>
using copy_lvalue_ref_t = std::conditional_t<
    std::is_lvalue_reference<U>::value,
    std::add_lvalue_reference_t<T>, T
>;

/// copy rvalue reference from U to T
template<class T, class U>
using copy_rvalue_ref_t = std::conditional_t<
    std::is_rvalue_reference<U>::value,
    std::add_rvalue_reference_t<T>, T
>;

/// copy lvalue or rvalue reference from U to T
template<class T, class U>
using copy_ref_t = copy_rvalue_ref_t<copy_lvalue_ref_t<T, U>, U>;



template<class T, class U, bool = std::is_pointer<U>::value>
struct copy_pointer_helper { using type = T; };

template<class T, class U>
struct copy_pointer_helper<T, U, true>
{
    using type = typename copy_pointer_helper<
        std::add_pointer_t<T>,
        std::remove_pointer_t<U>
    >::type;
};

/// copy all pointers from U to T (e.g. copy_ptrs_t<double, int**> -> double**)
template<class T, class U>
using copy_ptrs_t = typename copy_pointer_helper<T, U>::type;



/// copy the signedness of U to T
template<class T, class U>
using copy_signed_t = std::conditional_t<
    std::is_signed<U>::value, std::make_signed_t<T>, std::make_unsigned_t<T>>;



template<class T, class Seq = std::index_sequence<>>
struct get_extents_helper { using type = Seq; };

template<class T, std::size_t N, std::size_t ...I>
struct get_extents_helper<T[N], std::index_sequence<I...>>
{
    using type = typename get_extents_helper<T,
        std::index_sequence<N, I...>>::type;
};

template<class T, std::size_t ...I>
struct get_extents_helper<T[], std::index_sequence<I...>>
{
    using type = typename get_extents_helper<T,
        std::index_sequence<0, I...>>::type;
};

/// get the extents of T as an std::index_sequence in reverse order
/// (e.g. int[][1][2][3] -> <3, 2, 1, 0>)
template<class T>
using get_extents_t = typename get_extents_helper<T>::type;



template<class T, class Seq>
struct add_extents_helper { using type = T; };

template<class T, std::size_t I, std::size_t ...Is>
struct add_extents_helper<T, std::index_sequence<I, Is...>>
{
    using type = typename std::conditional_t<(I == 0),
        // note if there is a zero that's not at the end of the sequence,
        // the compiler will point towards here with an error about
        // incomplete type, add_extents_t takes a sequence in _reverse_ order
        add_extents_helper<T[], std::index_sequence<Is...>>,
        add_extents_helper<T[I], std::index_sequence<Is...>>
    >::type;
};

/// add extents to T in reverse order, corresponding to values given in an
/// std::index_sequence Seq (e.g.
/// add_extents_t<int, <3, 2, 1, 0>> -> int[][1][2][3])
template<class T, class Seq>
using add_extents_t = typename add_extents_helper<T, Seq>::type;



/// copy the array extents of U to T (e.g.
/// copy_extents_t<int, double[][1][2]> -> int[][1][2])
template<class T, class U>
using copy_extents_t = add_extents_t<T, get_extents_t<U>>;

} // namespace templ

#endif // TEMPL_BASIC_HPP
