template<typename T>
class Widget1 {

};

//-----------------------------------------------------------------------------
// Function template 1
template<typename T>
void f1(){}
// Explicit instantiation
template void f1<bool>(); // #1
// Implicit instantiations
void caller1(){
    f1<int>(); // #2
    f1<int>(); // #2 (same insts with same args only counted once)
}
template void f1<int>(); // #2

// Function template 2
template<template<typename T> class C = Widget1>
void f2(){
}
template void f2<Widget1>(); // #3

// Function template 3
template<void* N>
void f3(){};
template void f3<nullptr>(); // #4

// Function template 3
template<char... Cs>
void f4(){};
template void f4<'c'>(); // #5
template void f4<'p','p'>(); // #6

// RUN: clang++ %s -emit-ast -o %t1.ast
// RUN: %S/../../build/cxx-langstat --analyses=tia %t1.ast --
// RUN: diff %t1.ast.json %s.json
