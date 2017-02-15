//
// Created by cc on 1/20/17.
//

#ifndef TEMPL_SKEL_HPP
#define TEMPL_SKEL_HPP

#include "basic.hpp"
#include <templ/tstring.hpp>
#include "algorithm.hpp"

#include <type_traits>
#include <utility>

#include "basic.hpp"
#include <templ/tstring.hpp>
#include "algorithm.hpp"

namespace templ {

template<class Name, class T, T ptr,
    class = std::enable_if_t<std::is_member_object_pointer<T>::value>>
struct field_t
{
    using name = Name;
    using value_type = member_ptr_value_t<T>;
    using class_type = member_ptr_class_t<T>;

    static constexpr T member_ptr = ptr;

    /// get associated field from input object
    template<class U>
    static constexpr decltype(auto) get(U &&obj) noexcept {
        return std::forward<U>(obj).*ptr;}
};

template<class Obj, class Field, class ...Ignored>
struct field_return_impl :
    id<decltype(std::declval<Field>().get(std::declval<Obj>()))> {};

// alias template needs to have parameter pack as second template argument
// due to DR1430
template<class Obj, class ...Field>
using field_return_t = typename field_return_impl<Obj, Field...>::type;

// template comparator for fields (orders by template string name)
template<
    class NameA, class TA, TA ptrA,
    class NameB, class TB, TB ptrB
>
struct less<field_t<NameA, TA, ptrA>, field_t<NameB, TB, ptrB>> :
    less<NameA, NameB> {};

template<class U, U ptr, class Name>
constexpr field_t<Name, U, ptr> make_field(Name) {return {};}


template<class T>
struct set_functor
{
    T input;

    template<class U, class = std::enable_if_t<
            std::is_assignable<U, T>::value>>
    constexpr void operator()(U&& member) {
        std::forward<U>(member) = std::forward<T>(input);}
};

template<class T>
struct get_functor
{
    T output;

    template<class U, class = std::enable_if_t<
        std::is_assignable<T, U>::value>>
    constexpr void operator()(U&& member) {
        std::forward<T>(output) = std::forward<U>(member);}
};

// decltype(skel<foo>::at<skel<foo>::field_id("name")>);

//template<std::size_t Idx, class T>
//struct leaf
//{
//    T value;
//};
//
//template<class, class...>
//struct tuplet_impl;
//
//template<class ...Ts, std::size_t ...I>
//struct tuplet_impl<std::index_sequence<I...>, Ts...> :
//    public leaf<I, Ts>...
//{
//    tuplet_impl(Ts ...args) :
//        leaf<I, Ts>{std::forward<Ts>(args)}... {}
//};
//
//template<class ...Ts>
//struct tuplet
//{
//    tuplet_impl<std::make_index_sequence<sizeof...(Ts)>, Ts...> impl;
//
//    tuplet(Ts...args) : impl(std::forward<Ts>(args)...) {}
//};
//
//tuplet<int, char, std::string> t(1, 'c', "str");

template<class Field, class Obj>
struct leaf
{
    static constexpr decltype(auto) get(Obj obj, const char *name) noexcept
    {
        if (Field::name::cmp(name) == 0)
            return std::forward<Obj>(obj).*Field::member_ptr;
    }
};

template<class O, class ...Fields>
struct intermediary_t
{
    O obj;
    const char *name;

    template<std::size_t N>
    using at = templ::at_t<pack<Fields...>, N>;

    template<class T, class Field>
    using is_get_compatible = typename compose<
        partial<std::is_assignable, T>::template type,
        partial<field_return_t, O>::template type
    >::template type<Field>;

    template<template<class> class Predicate>
    using compatible_intermediary = copy_if_t<
        pack<Fields...>, intermediary_t<O>, Predicate>;


    static constexpr std::size_t size() {return sizeof...(Fields);}

    template<
        class T,
        class Compat = compatible_intermediary<
            partial<is_get_compatible, T>::template type>,
        class = std::enable_if_t<(!empty_v<Compat> && size_v<Compat> != size())>
    >
    constexpr operator T() {
        return Compat{std::forward<O>(obj)};
    }


    template<
        class T,
        class = std::enable_if_t<all_of_v<
            pack<Fields...>,
            compose<
                partial<std::is_assignable, T>::template type,
                partial<field_return_t, O>::template type
            >::template type
        >>
    >
    constexpr operator T() {
        return get_impl<T>();
    }

    template<
        class T, int L = 0, int R = size(),
        class = std::enable_if_t<(L < R)>
    >
    constexpr T get_impl()
    {
        constexpr auto idx = (L + R) / 2;
        int cmp = at<idx>::cmp(name);

        if (cmp < 0)
            return get_impl<T, L, idx - 1>();
        else if (cmp > 0)
            return get_impl<T, idx + 1, R>();

        return std::forward<O>(obj).*at<idx>::member_ptr;
    }

    template<class T, int L, int R>
    constexpr T get_impl(
        std::enable_if_t<(L >= R)>* = nullptr)
    {
        return std::forward<O>(obj).*at<L>::member_ptr;
    }
};

template<class ...Fields>
struct skeleton_t
{
    /// type alias which returns the field type at the specified index
    template<std::size_t N>
    using at = templ::at_t<pack<Fields...>, N>;

    /// type alias which returns the field type with the specified tstring Name
    template<class Name>
    using field = at<templ::find_v<pack<Fields...>, Name>>;

    /// helper template that gives a skeleton_t<> that only contains fields
    /// that can be passed to a given functor of type F assuming object type O
    template<class F, class O>
    using compatible_skel = copy_if_t<
        pack<Fields...>, skeleton_t<>,
        compose<
            partial<is_callable, F>::template type,
            partial<field_return_t, O>::template type
        >::template type
    >;

    /// number of fields
    static constexpr std::size_t size() {return sizeof...(Fields);}

    /// get the field id (numeric) for the given field name
    static constexpr std::size_t field_id(const char *name) {
        return field_id_impl(name);}


    /// apply the given functor to each field, passing the member variable
    /// associated with the field and object as the argument
    template<class T, class F>
    static constexpr void for_each(T&& obj, F&& func)
    {
        compatible_skel<F, T>::for_each_impl(
            std::forward<T>(obj), std::forward<F>(func)
        );
    }

    /// assign the value of the input variable to the field with the given name
    template<class T, class U>
    static constexpr void set(T&& obj, const char *name, U&& input)
    {
        apply(
            std::forward<T>(obj), name,
            set_functor<U>{std::forward<U>(input)}
        );
    }

    /// assign the value of the field with the given name to the output variable
    template<class T, class U>
    static constexpr void get(T&& obj, const char *name, U&& output)
    {
        apply(
            std::forward<T>(obj), name,
            get_functor<U>{std::forward<U>(output)}
        );
    }

    template<class T>
    static constexpr auto get_int(T&& obj, const char *name)
    {
        return intermediary_t<T, Fields...>{std::forward<T>(obj), name};
    }

    /// binary search to apply functor
    template<class T, class F>
    static constexpr void apply(T&& obj, const char *name, F&& func)
    {
        compatible_skel<F, T>::apply_impl(
            std::forward<T>(obj), name, std::forward<F>(func));
    }

////////////////////////////////////////////////////////////////////////////////
// Implementations                                                            //
////////////////////////////////////////////////////////////////////////////////

    template<
        size_t N = 0, class O, class F,
        class = std::enable_if_t<(N < size())>
    >
    static constexpr void for_each_impl(O&& obj, F&& func)
    {
        // apply
        std::forward<F>(func)(at<N>::get(std::forward<O>(obj)));
        // move to the next one
        for_each<N + 1>(std::forward<O>(obj), std::forward<F>(func));
    }

    template<size_t N = 0, class O, class F>
    static constexpr void for_each_impl(O&&, F&&,
        std::enable_if_t<(N >= size())>* = nullptr) {}

    template<
        int L = 0, int R = size(),
        class = std::enable_if_t<(L < R)>
    >
    static constexpr std::size_t field_id_impl(const char *name)
    {
        constexpr auto idx = (L + R) / 2;
        int cmp = at<idx>::cmp(name);

        if (cmp < 0)
            return field_id_impl<L, idx - 1>(name);
        else if (cmp > 0)
            return field_id_impl<idx + 1, R>(name);

        return idx;
    }

    template<int L, int R>
    static constexpr std::size_t field_id_impl(const char*,
        std::enable_if_t<(L >= R)>* = nullptr) {return 0;}

    template<
        int L = 0, int R = size(),
        class T, class F,
        class = std::enable_if_t<(L < R)>
    >
    static constexpr void apply_impl(T&& obj, const char *name, F&& func)
    {
        constexpr auto idx = (L + R) / 2;
        using field_type = at<idx>;

        auto cmp = field_type::name::cmp(name);

        if (cmp < 0)
        {
            apply_impl<L, idx - 1>(std::forward<T>(obj), name,
                std::forward<F>(func));
        }
        else if (cmp > 0)
        {
            apply_impl<idx + 1, R>(std::forward<T>(obj), name,
                std::forward<F>(func));
        }
        else
        {
            std::forward<F>(func)(field_type::get(std::forward<T>(obj)));
        }
    }

    template<int L, int R, class F, class T>
    static constexpr void apply_impl(T&&, const char*, F&&,
        std::enable_if_t<(L >= R)>* = nullptr) {}
};

/// 'construct' skeleton type from list of fields
template<typename ...Fields>
constexpr auto make_skeleton(Fields...) {
    // note that we sort the fields (by name) in a pack<> struct and
    // then move them into the skeleton struct
    return templ::construct_t<sort_t<pack<Fields...>>, skeleton_t>{};
}

template<typename T>
using skel = decltype(define_skeleton(std::declval<std::decay_t<T>>()));

#define SKEL_FIELD(name, var)                                                  \
templ::make_field<decltype(&var), &var>(MAKE_TSTRING(name))

} // namespace src

#endif // TEMPL_SKEL_HPP
