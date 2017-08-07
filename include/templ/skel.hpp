#ifndef TEMPL_SKEL_HPP
#define TEMPL_SKEL_HPP

#include "basic.hpp"
#include "tstring.hpp"
#include "algorithm.hpp"

#include <type_traits>
#include <utility>
#include <iostream>

namespace templ {

template<class Name, class T, T ptr,
    class = std::enable_if_t<std::is_member_object_pointer<T>::value>>
struct field_t
{
    using name = Name;
    using value_type = member_ptr_object_t<T>;
    using class_type = member_ptr_class_t<T>;

    static constexpr T member_ptr = ptr;

    /// get associated field from input object
    template<class U>
    static constexpr decltype(auto) get(U &&obj) noexcept {
        return std::forward<U>(obj).*ptr;}
};

template<class Obj, class Field, class ...>
struct field_return_impl :
    id<decltype(Field::template get<Obj>(std::declval<Obj>()))> {};

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


namespace detail {

template<typename O, typename T, typename Field,
    bool = std::is_assignable<field_return_t<O, Field>, T>::value>
struct get_field_setter : std::integral_constant<std::nullptr_t, nullptr> {};

template<typename O, typename T, typename Field>
struct get_field_setter<O, T, Field, true>
{
    static void field_set(O object, T input) {
        Field::get(std::forward<O>(object)) = std::forward<T>(input); }

    static constexpr auto value = field_set;
};

template<typename O, typename  T, typename ...Fields>
inline void field_set_jtable(std::size_t field_idx, O object, T value)
{
    using fn_t = void(*)(O, T);
    static constexpr fn_t jump_table[] = {
        get_field_setter<O, T, Fields>::value...
    };

    jump_table[field_idx](object, value);
}



template<typename O, typename T, typename Field,
    bool = std::is_convertible<field_return_t<O, Field>, T>::value>
struct get_field_getter : std::integral_constant<std::nullptr_t, nullptr> {};

template<typename O, typename T, typename Field>
struct get_field_getter<O, T, Field, true>
{
    static T field_get(O object) {
        return Field::get(std::forward<O>(object)); }

    static constexpr auto value = field_get;
};

template<typename O, typename  T, typename ...Fields>
inline T field_get_jtable(std::size_t field_idx, O object)
{
    using fn_t = T(*)(O);
    static constexpr fn_t jump_table[] = {
        get_field_getter<O, T, Fields>::value...
    };

    return jump_table[field_idx](object);
}

} // namespace detail

// TODO: Make private constructor, non-copyable, etc
template<class O, class ...Fields>
struct intermediary_t
{
    O obj;
    const std::size_t field_idx;

    template<typename T>
    T as() {
        return detail::field_get_jtable<O, T, Fields...>(field_idx, obj);
    }

    template<class T>
    void operator=(T&& value)
    {
        detail::field_set_jtable<O, T, Fields...>(
            field_idx, obj, std::forward<T>(value));
    }
};

template<class O, class ...Fields>
struct skeleton_t
{
    O object;

    /// type alias which returns the field type at the specified index
    template<std::size_t N>
    using at = templ::at_t<pack<Fields...>, N>;

    /// type alias which returns the field type with the specified tstring Name
    template<class Name>
    using field = at<templ::find_v<pack<Fields...>, Name>>;

    /// helper template that gives a skeleton_t<> that only contains fields
    /// that can be passed to a given functor of type F assuming object type O
    template<class F>
    using func_compatible_skel = copy_if_t<
        pack<Fields...>, skeleton_t<O>,
        compose<
            partial<is_callable, F>::template type,
            partial<field_return_t, O>::template type
        >::template type
    >;

    /// number of fields
    static constexpr std::size_t size() {return sizeof...(Fields);}

    /// whether the skeleton is empty, aka size() == 0
    static constexpr bool empty() {return size() == 0;}

    /// get an intermediary type for setting/getting fields
    auto operator[](const char *name)
    {
        return intermediary_t<O, Fields...>{
            object, find(name)
        };
    }

    /// get the field id (numeric) for the given field name
    static constexpr std::size_t find(const char *name) {
        return find_impl(name);}


    /// apply the given functor to each field, passing the member variable
    /// associated with the field and object as the argument
    template<class F>
    constexpr void for_each(F&& func)
    {
        for_each_impl(
            std::forward<F>(func)
        );
    }


////////////////////////////////////////////////////////////////////////////////
// Implementations                                                            //
////////////////////////////////////////////////////////////////////////////////

    template<class Field, class F>
    auto apply_impl(F& func)
        -> void_t<decltype(
            func(
                Field::name::as_std_string(),
                Field::get(std::forward<O>(object))
            )
        )>
    {
        func(
            Field::name::as_std_string(),
            Field::get(std::forward<O>(object))
        );
    }

    template<class Field, class F>
    constexpr void apply_impl(F&&) {}

    template<
        std::size_t N = 0, class F,
        class = std::enable_if_t<(N < size())>
    >
    constexpr void for_each_impl(F&& func)
    {
        // apply
        apply_impl<at<N>>(func);
        // move to the next one
        for_each_impl<N + 1>(std::forward<F>(func));
    }

    template<std::size_t N = 0, class F>
    constexpr void for_each_impl(F&&,
        std::enable_if_t<(N >= size())>* = nullptr) {}

    template<
        std::size_t  L = 0, std::size_t  R = size(),
        class = std::enable_if_t<(L < R)>
    >
    static constexpr std::size_t find_impl(const char *name)
    {
        constexpr auto idx = L + (R - L) / 2;
        const auto cmp = at<idx>::name::cmp(name);

        if (cmp < 0)
            return find_impl<idx + 1, R>(name);
        else if (cmp > 0)
            return find_impl<L, idx>(name);
        else
            return idx;
    }

    template<std::size_t L, std::size_t R>
    static constexpr std::size_t find_impl(const char*,
        std::enable_if_t<(L >= R)>* = nullptr) { return size(); }
};

/// 'construct' skeleton type from list of fields
template<typename ...Fields>
constexpr auto make_skeleton(Fields...) {
    // note that we sort the fields (by name) in a pack<> struct and
    // then move them into the skeleton struct
    return sort_t<pack<Fields...>>{};
}

template<typename T>
using define_skel_result_t =
    decltype(define_skeleton(std::declval<std::decay_t<T>>()));

template<typename T>
using skel_type = concat_t<skeleton_t<T>, define_skel_result_t<T>>;

template<typename T, typename = void_t<>>
struct has_skeleton : std::false_type {};

template<typename T>
struct has_skeleton<T, void_t<define_skel_result_t<T>>> :
    std::true_type {};

template<typename T>
constexpr bool has_skeleton_v = has_skeleton<T>::value;

template<typename T>
auto skel(T&& object) {
    return skel_type<T>{std::forward<T>(object)};
}

template<class U, U ptr, class Name>
auto make_field_sname(Name)
{
    constexpr auto colon_pos = Name::find_last_of(':'),
        substr_pos = (colon_pos == std::string::npos) ?
                        0 : std::min(Name::size(), colon_pos + 1);

    return make_field<U, ptr>(Name::template substr<substr_pos>());
}

/// remove namespace(s) from name, via finding last colon
template<typename Name>
using remove_ns_t = decltype(
    Name::template substr<
        Name::find_last_of(':') == std::string::npos ?
            0 : std::min(Name::size(), Name::find_last_of(':') + 1)
    >()
);

#define SKEL_FIELD_AUTO(var)                                                   \
templ::make_field_sname<decltype(&var), &var>(MAKE_TSTRING(#var))

#define SKEL_FIELD(var, name)                                                  \
templ::make_field<decltype(&var), &var>(MAKE_TSTRING(name))

///// Usage ////////////////////////////////////////////////////////////////////
//
// struct test
// {
//     std::string name = "Fred";
//     int integer = -1;
//     double pi = 3.14159;
//     std::vector<double> vec = {1.0, 2.0, 3.0};
// };
//
// // should be either in the global namespace or visible via ADL
// auto define_skeleton(test&&)
// {
//     return templ::make_skeleton(
//         SKEL_FIELD_AUTO(test::integer),
//         SKEL_FIELD_AUTO(test::vec),
//         SKEL_FIELD(test::name, "name_str"),
//         SKEL_FIELD(test::pi, "PI")
//     );
// }
//
// int main(int argc, char *argv[])
// {
//     test a;
//     auto s = templ::skel(a);
//
//     std::cout << "name: " << a.name << std::endl;
//     s["name_str"] = "Bob";
//     std::cout << "name: " << s["name_str"].as<std::string>() << std::endl;
//
//     int &iref = s["integer"].as<int&>();
//     iref = 0;
//     std::cout << "\ninteger: " << a.integer << std::endl;
//     s["integer"] = -1;
//
//     std::cout << "\nFields: " << std::endl;
//     s.for_each([](std::string str, auto &&v)
//         -> templ::void_t<decltype(std::cout << v)> {
//         std::cout << str << ": " << v << std::endl;
//     });
//
//     return 0;
// }
//
///// Output ///////////////////////////////////////////////////////////////////
//
// name: Fred
// name: Bob
//
// integer: 0
//
// Fields:
// PI: 3.14159
// integer: -1
// name_str: Bob
//
////////////////////////////////////////////////////////////////////////////////

} // namespace templ

#endif // TEMPL_SKEL_HPP
