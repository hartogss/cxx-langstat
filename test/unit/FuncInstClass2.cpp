// RUN: clang++ %s -emit-ast -o %t1.ast
// RUN: %S/../../build/cxx-langstat --analyses=tia --store %t1.ast --
// RUN: diff %t1.ast.json %s.json

// Test what happens when instantiating function templates that instantiate
// classes inside them. The subtle point is that we don't want to count class
// instantiations that are inside function templates. We only count them inside
// the instantiation function template.
// Same kind of example as FuncInstClass.cpp, but with template template argument.

template<typename T>
class Widget1 {
};

template<typename T>
class Widget2 {
};

// Function template
template<template<typename T> class C = Widget1>
void f3(){
    C<char> c; // once isnt'd as Widget1<char> by f3<Widget1>, once as Widget2<char> by f3<Widget2>.
    Widget1<int> w1; // once inst'd by f3<Widget1>, once by f3<Widget2>.
}
void caller3(){
    f3(); // Causes single instantiation of f3 with default arg
    f3<Widget1>(); // no extra instantiation, since same as on line above
    f3<Widget2>();
}
