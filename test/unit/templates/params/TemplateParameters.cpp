// RUN: clang++ %s -emit-ast -o %t1.ast
// RUN: %S/../../../../build/cxx-langstat --analyses=tpa %t1.ast --
// RUN: diff %t1.ast.json %s.json

// Test to see if location, whether it uses param pack or not,
// correct counts for each kind of param are reported.
//

// Class templates
template<int N> // √
class Widget1 {};

template<int N, int M> // √
class Widget1_1 {};

template<typename T> // √
class Widget2 {};

template<template<typename T> class c> // √
class Widget3 {};

template<template<int ...N> class c> // √
class Widget4 {};

template<int ...N> // √
class Widget1_p {};

template<typename ...T> // √
class Widget2_p {};

template<template<typename T> class...> // √
class Widget3_p {};

// ----------------------------------------------------------------------------
// Function templates
template<typename ...T, typename ...U>  // √
void func1(){}
class Widget_f1 {
    template<template<unsigned N> class c>  // √
    void method1();
};

// ----------------------------------------------------------------------------
// Variable templates
template<typename T, int N> // √
T num = N;

// ----------------------------------------------------------------------------
// Alias templates
template<int N> // √
using Alias1 = Widget1<N>;

template<int N, int M> // √
using Alias1_1 = Widget1_1<N, M>;

template<typename T> // √
using Alias2 = Widget2<T>;

template<unsigned ...Ns> // √
using cAlias1 = Widget1_p<Ns...>;

template<typename T> // √
using cAlias2 = Widget2_p<T>;

template<template <typename T> class c> // √
using cAlias3 = Widget3<c>;

// ----------------------------------------------------------------------------
// Mixing templates
// Class template with function/method template
template<typename U> // √
class Widget_m1 {
    template<template<unsigned N> class c>  // √
    void method1();
};
// Class template with variable template
template<typename U> // √
class Widget_m2 {
    template<template<unsigned N> class c>  // √
    static U data;
};


int main(int argc, char** argv){
    Widget1<3> w1;
    Widget2_p<int, unsigned> w2;
    Alias1<2> w;
    cAlias3<Widget2> w3;
    Widget4<Widget1_p> w4;

}
