//
// immer: immutable data structures for C++
// Copyright (C) 2016, 2017, 2018 Juan Pedro Bolivar Puente
//
// This software is distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE or copy at http://boost.org/LICENSE_1_0.txt
//

#pragma once

#include <immer/config.hpp>

#include <cstddef>
#include <new>
#include <type_traits>
#include <memory>

namespace immer {
namespace detail {

template <typename T>
using aligned_storage_for =
    typename std::aligned_storage<sizeof(T), alignof(T)>::type;

template <typename T>
T& auto_const_cast(const T& x) { return const_cast<T&>(x); }
template <typename T>
T&& auto_const_cast(const T&& x) { return const_cast<T&&>(std::move(x)); }

template <typename Iter1, typename Iter2>
auto uninitialized_move(Iter1 in1, Iter1 in2, Iter2 out)
{
    return std::uninitialized_copy(std::make_move_iterator(in1),
                                   std::make_move_iterator(in2),
                                   out);
}

template <class T>
void destroy(T* first, T* last)
{
    for (; first != last; ++first)
        first->~T();
}

template <class T, class Size>
void destroy_n(T* p, Size n)
{
    auto e = p + n;
    for (; p != e; ++p)
        p->~T();
}

template <typename Heap, typename T, typename... Args>
T* make(Args&& ...args)
{
    auto ptr = Heap::allocate(sizeof(T));
    try {
        return new (ptr) T{std::forward<Args>(args)...};
    } catch (...) {
        Heap::deallocate(sizeof(T), ptr);
        throw;
    }
}

struct not_supported_t {};
struct empty_t {};

template <typename T>
struct exact_t
{
    T value;
    exact_t(T v) : value{v} {};
};

template <typename T>
inline constexpr auto log2(T x)
{
    return std::log2(x);
}


template <bool b, typename F>
auto static_if(F&& f) -> std::enable_if_t<b>
{ std::forward<F>(f)(empty_t{}); }
template <bool b, typename F>
auto static_if(F&& f) -> std::enable_if_t<!b>
{}

template <bool b, typename R=void, typename F1, typename F2>
auto static_if(F1&& f1, F2&& f2) -> std::enable_if_t<b, R>
{ return std::forward<F1>(f1)(empty_t{}); }
template <bool b, typename R=void, typename F1, typename F2>
auto static_if(F1&& f1, F2&& f2) -> std::enable_if_t<!b, R>
{ return std::forward<F2>(f2)(empty_t{}); }

template <typename T, T value>
struct constantly
{
    template <typename... Args>
    T operator() (Args&&...) const { return value; }
};

} // namespace detail
} // namespace immer
