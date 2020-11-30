template<int N>
class Widget1 {};

template<int N, int M>
class Widget1_1 {};

template<typename T>
class Widget2 {};

template<template<typename T> class c>
class Widget3 {};

template<template<int ...N> class c>
class Widget4 {};

template<int ...N>
class Widget1_p {};

template<typename ...T>
class Widget2_p {};

template<template<typename T> class...>
class Widget3_p {};

// ----------------------------------------------------------------------------

template<int N>
using Alias1 = Widget1<N>;

template<int N, int M>
using Alias1_1 = Widget1_1<N, M>;

template<typename T>
using Alias2 = Widget2<T>;

template<unsigned ...Ns>
using cAlias1 = Widget1_p<Ns...>;

template<typename T>
using cAlias2 = Widget2_p<T>;

template<template <typename T> class c>
using cAlias3 = Widget3<c>;



int main(int argc, char** argv){
    Widget1<3> w1;
    Widget2_p<int, unsigned> w2;
    Alias1<2> w;
    cAlias3<Widget2> w3;
    Widget4<Widget1_p> w4;

}
