// RUN: clang++ %s -emit-ast -o %t1.ast
// RUN: %S/../../../../../build/cxx-langstat --analyses=tia -emit-features -in %t1.ast -out %t1.ast.json --
// RUN: diff %t1.ast.json %s.json

// Basic tests of variable templates instantiations at namespace scope.
//
//

// Since C++20 this could also be a floating point non-type template parameter
// Test that exp/imp instantiation that comes first is reported.
template<int N>
int data = N;
// Explicit instantiation
template int data<0>;
// Implicit instantiation
void caller(){
    data<0>;
}
//
template<int N>
int data2 = N;
void caller2(){
    data2<42>;
}
template int data2<42>;


// Test that only the first instantiation is reported
template<int N>
int data3;
void caller3(){
    data3<0>;
    data3<0>;
    data3<1>;
}

// Test that template specialization are not matched
template<>
int data3<42>;


// Example from https://en.cppreference.com/w/cpp/language/variable_template
template<typename T>
constexpr T pi = T(3.1415926535897932385L);
void callerpi(){
    pi<float>;
    pi<double>;
    pi<double>;
    int test = pi<int> + pi<int>;
}


// Some more params
template<int N, int M, typename... Ts>
int data4 = N;
template int data4<3, 42, double>;
void caller4(){
    data4<14, pi<int>>; // possible because pi is constexpr
}
