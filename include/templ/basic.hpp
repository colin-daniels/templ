//
// Created by cc on 1/20/17.
//

#ifndef TEMPL_BASIC_HPP
#define TEMPL_BASIC_HPP

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



/// utility 'any' class, can be constructed or converted to any type
struct any_t
{
    any_t() = default;

    template<class T>
    any_t(T&&);

    template<class T>
    any_t& operator=(T&&);

    template<class T>
    operator T();
};

/// std::size_t integral constant
template<std::size_t N>
using ic_size_t = std::integral_constant<std::size_t, N>;

template<bool ok>
struct tmp_assert {static_assert(ok, "");};

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
//     src::pack<int, void>
// >::template type<char, float>
//     is equivalent to
// src::pack<char, float>

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

template<class T>
constexpr auto false_v = false;

/// helper alias for repeating types via sequences
template<class T, std::size_t>
using repeat_helper_t = T;

// TODO: rename or something idk -_-
template<class...>
struct repeat_helper2;

template<class T, class ...Ts>
struct repeat_helper2<T, Ts...>
{
    using type = T;
};

/// helper alias for repeating types via type parameter packs
template<class ...Ts>
using repeat_helper2_t = typename repeat_helper2<Ts...>::type;

// TODO: is_constexpr
//template<class, class = void_t<>>
//struct is_constexpr_impl : std::false_type {};
//
//template<
//    class F, class ...Args // Args assumed to be integral constants
//>
//struct is_constexpr_impl<pack<F, Args...>,
//    void_t<std::integral_constant<int, (F(Args...)>> :
//    std::true_type {};
//
//namespace detail {
//template<int>
//struct sfinae_true : std::true_type {};
//
//template<class T>
//sfinae_true<(T::f(), 0)> check(int);
//
//template<class>
//std::false_type check(...);
//} // detail::
//
//template<class T>
//struct has_constexpr_f : decltype(detail::check<T>(0)){};

} // namespace src

#endif // TEMPL_BASIC_HPP
