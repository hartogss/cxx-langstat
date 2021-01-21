// RUN: clang++ %s -emit-ast -o %t1.ast
// RUN: %S/../../../../../build/cxx-langstat --analyses=tia -emit-features -in %t1.ast -out %t1.ast.json --
// RUN: diff %t1.ast.json %s.json

// Test file to check if basic class instantiations are correctly found & each and
// every one of them is detected.
//

// Basic tests, check that explicit and implicit instantiations (varDecl,
// parameter decl, field decl) are all reported, and reported multiple times
// for each implicit use.
template<typename T>
class Widget1 {

};
// Explicit specialization,
// not what we want to match here
template<>
class Widget1<int> {
    int x;
};
// Explicit instantiation
template class Widget1<float>;
// template class Widget1<float>; // not possible
// Implicit instantiations
// As VarDecl
Widget1<double> w1;
Widget1<double> w1_1; // Should yield second instantiation entry in JSON
Widget1<float> w1_2; // Explicit equiv. instantiation above should not cause this one to be not reported
// As ParmVarDecls, which is also a VarDecl, so no extra work needed.
void f(Widget1<char> w3){
}
// As FieldDecl, both of which should be reported.
class Helper {
    Widget1<void> wf;
    Widget1<void> wf_;
};


// Test that for parameter packs all arguments given to template are reported
template<typename ...Ts>
class Widget1_p {};
template class Widget1_p<int>;
template class Widget1_p<int, int>;
template class Widget1_p<int, double, float, unsigned>;
Widget1_p<double, double> w1p;


//
template<int N, int ...Ms>
class Widget2_p{};
template class Widget2_p<3,1,4>;
constexpr int call(){
    return 3;
}
template class Widget2_p<call()>;


// Class Template 3. Important test to check interaction of insts of same class with same param.
template<template<typename T> class C>
class Widget3 {};
Widget3<Widget1> w3_1;
template class Widget3<Widget1>;
Widget3<Widget1> w3;
