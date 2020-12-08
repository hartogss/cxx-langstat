template<typename T>
class Widget1 {

};

//-----------------------------------------------------------------------------
// Function template 1
template<typename T>
void f1(){}
// Explicit instantiation
template void f1<bool>();
// Implicit instantiations
void caller1(){
    f1<int>();
}

// Function template 2
template<template<typename T> class C = Widget1>
void f2(){
    C<int>();
}
template void f2<Widget1>();
void caller2(){
    f2(); // don't need template template argument thanks to default arg
}

// Function template 3
template<template<typename T> class C = Widget1, template<typename T> class C1 = Widget1>
void f3(){
    C<int>();
}
template void f3<Widget1, Widget1>();

// Function template 4
template<void* N>
void f4(){};
template void f4<nullptr>();
