/*
 * This file is part of the EMBTOM project
 * Copyright (c) 2018-2020 Thomas Willetal
 * (https://github.com/embtom)
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef TEMPLATEHELPERS_H
#define TEMPLATEHELPERS_H

#include <type_traits>
#include <array>
#include <tuple>
#include <span.h>

namespace utils
{

/////////////////////////////////////////////////////////
//type trait to check if span type

template<typename T>
struct is_span :  std::false_type{};

template<typename T>
struct is_span<utils::span<T>> // : std::true_type {};
{
    static constexpr bool value = std::is_trivially_copyable_v<T>;
};

template <typename T>
inline constexpr bool is_span_v = is_span<T>::value;

/////////////////////////////////////////////////////////
//to check if pass TYPE is a is std::array
template<typename T>
struct is_array :  std::false_type {};

template<typename T, std::size_t N>
struct is_array<std::array<T, N>> : std::true_type {};

/////////////////////////////////////////////////////////
//type trait to get array count of c-style array type
template<typename T, typename Enable = void>
struct array_count
{
    static constexpr int value = 1;
};

template<typename T>
struct array_count<T, typename std::enable_if<std::is_array<T>::value>::type>
{
    static constexpr int value = sizeof(T) / sizeof(typename std::remove_pointer_t<typename std::decay_t<T>>);
};

template <typename T>
inline constexpr int array_count_v = array_count<T>::value;

/////////////////////////////////////////////////////////
//remove cont, volatile, and reference from passed type
template< class T >
struct remove_cvref {
    typedef std::remove_cv_t<std::remove_reference_t<T>> type;
};

template< class T >
using remove_cvref_t = typename remove_cvref<T>::type;

/////////////////////////////////////////////////////////
//type trait to check if string type
template <typename T>
struct is_string: public std::integral_constant<bool,
        std::is_same_v<char*, typename std::decay_t<T>> ||
        std::is_same_v<const char*, typename std::decay_t<T>> ||
        std::is_same_v<std::string, typename std::decay_t<T>>> { };

template <>
struct is_string<std::string> : std::false_type {};

template<typename T>
inline constexpr bool is_string_v = is_string<T>::value;

/////////////////////////////////////////////////////////
//type trait to check if forward declaration
template <typename, typename = size_t>
struct is_complete : std::false_type {};

template <typename T>
struct is_complete<T, decltype(sizeof(T))> : std::true_type {};

template <typename _Tp>
inline constexpr bool is_complete_v = is_complete<_Tp>::value;

/////////////////////////////////////////////////////////
// for_each_arg - call f for each element from tuple
template <typename F, typename... Args, std::size_t... Idx>
void for_tuple_impl(F&& f, const std::tuple<Args...>& tuple, std::index_sequence<Idx...>)
{
    (f(std::get<Idx>(tuple)), ...);
}

template <typename F, typename... Args>
void for_tuple(F&& f, const std::tuple<Args...>& tuple)
{
    for_tuple_impl(f, tuple, std::index_sequence_for<Args...>{});
}

template <typename F>
void for_tuple(F&& /* f */, const std::tuple<>& /* tuple */)
{ /* do nothing */ }

} //utils
#endif //TEMPLATEHELPERS_H