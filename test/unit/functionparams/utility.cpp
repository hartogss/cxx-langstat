// RUN: rm %t1.ast.json || true
// RUN: %clangxx %s -emit-ast -o %t1.ast
// RUN: %cxx-langstat --analyses=fpa -emit-features -in %t1.ast -out %t1.ast.json --
// RUN: diff %t1.ast.json %s.json
// Of all functions we should test with FPA, it should be std::move
// and std::forward because of meta.
//

// Declarations ripped from
// https://en.cppreference.com/w/cpp/utility/move and
// https://en.cppreference.com/w/cpp/utility/forward respectively.

#include <type_traits>

// std::move declaration from cpp standard
// since C++11, until C++14
template<class T>
typename std::remove_reference<T>::type&&
move_cpp11( T&& t ) noexcept;
// since C++14
template<class T>
constexpr std::remove_reference_t<T>&&
move_cpp14( T&& t ) noexcept;


// std::forward
// since C++11, until C++14
template<class T>
T&& forward_cpp11l( typename std::remove_reference<T>::type& t ) noexcept;
// since C++14
template<class T>
constexpr T&& forward_cpp14l( std::remove_reference_t<T>& t ) noexcept;
// since C++11, until C++14
template<class T>
T&& forward_cpp11r( typename std::remove_reference<T>::type&& t ) noexcept;
// since C++14
template<class T>
constexpr T&& forward_cpp14r( std::remove_reference_t<T>&& t ) noexcept;
