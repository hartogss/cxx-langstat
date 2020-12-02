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
template class Widget1<float>;
// Implicit instantiation
Widget1<double> w1;
Widget1<unsigned> w2;

// Class template 2
template<typename ...Ts>
class Widget1_p {};
// Explicit
template class Widget1_p<int, int>;
template class Widget1_p<Widget1<double>>;
template class Widget1_p<int, double, float, unsigned, Widget1<int>>;
// Implicit
Widget1_p<double, Widget1<int>> w1p;

template<int N, int ...Ms>
class Widget2_p{};
template class Widget2_p<3,1,4>;

constexpr int call(){
    return 3;
}
template class Widget2_p<call()>;
// template class Widget2_p<NULL>;


//-----------------------------------------------------------------------------
// Function template 1
template<typename T>
void f1(){}
// Explicit instantiation
template void f1<bool>();

// Function template 2
template<template<typename T> class C = Widget1>
void f2(){
    C<int>();
}
template void f2<Widget1>();
// Implicit instantiations
void caller(){
    f1<int>();
    f2(); // don't need template template argument thanks to default arg
}
// Function template 3
template<template<typename T> class C = Widget1, template<typename T> class C1 = Widget1>
void f3(){
    C<int>();
}
template void f3<Widget1, Widget1>();

//
template<void* N>
void f4(){};
template void f4<nullptr>();


//-----------------------------------------------------------------------------
// Variable template 1
// Since C++20 this could also be a floating point non-type template parameter
template<int N>
int data = N;
// Explicit instantiation
template int data<0>;

// Variable template 2
template<int N>
int data2 = N;
// Implicit instantiations
int caller2(){
    return data2<1>;
}

// Variable template 3
// https://en.cppreference.com/w/cpp/language/variable_template
template<typename T>
constexpr T pi = T(3.1415926535897932385L);
// Implicit instantiation
void caller3(){
    pi<float>;
    pi<double>;
}

// Variable template 4
template<int N, int M, typename T>
int data4 = N;
template int data4<3, 42, double>;
