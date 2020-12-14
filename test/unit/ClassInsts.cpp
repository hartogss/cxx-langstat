// RUN: clang++ %s -emit-ast -o %t1.ast
// RUN: %S/../../build/cxx-langstat --analyses=tia %t1.ast --
// RUN: diff %t1.ast.json %s.json

// Test file to check if class instantiations are correctly found & each and
// everyone of them is detected.

// Class template 1
template<typename T>
class Widget1 {

};
// Explicit specialization
// Not what we want to match here
template<>
class Widget1<int> {
    int x;
};
// Explicit instantiation
template class Widget1<float>; // #1
// template class Widget1<float>; // not possible
// Implicit instantiations
// As VarDecl
Widget1<double> w1; // #2
Widget1<double> w1_1; // #3. Should yield snd instantiation entry in JSON
Widget1<Widget1<bool>> w2; // #4. 'Subinstantiation' not registered.
// As ParmVarDecls, which is also a VarDecl, so no extra work needed.
void f(Widget1<char> w3){ // #5
}
// As FieldDecl
class Helper {
    Widget1<void> wf; // #6
};

class Helper2 {
    Widget1<void> wf; // #7
};

// Class template 2
template<typename ...Ts>
class Widget1_p {};
// Explicit
template class Widget1_p<int, int>; // #8
template class Widget1_p<Widget1<double>>; // #9
template class Widget1_p<int, double, float, unsigned, Widget1<int>>; // #10
// Implicit
Widget1_p<double, Widget1<int>> w1p; // #11

template<int N, int ...Ms>
class Widget2_p{};
template class Widget2_p<3,1,4>; // #12

constexpr int call(){
    return 3;
}
template class Widget2_p<call()>; // #13

// Class Template 3. Important test to check interaction of insts of same class with same param.
template<template<typename T> class C>
class Widget3 {};
Widget3<Widget1> w3_1; // #14
template class Widget3<Widget1>; // #15
Widget3<Widget1> w3; // #16
