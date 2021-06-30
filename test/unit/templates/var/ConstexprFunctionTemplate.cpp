// RUN: rm %t1.ast.json || true
// RUN: %clangxx %s -emit-ast -o %t1.ast
// RUN: %cxx-langstat --analyses=vta -emit-features -in %t1.ast -out %t1.ast.json --
// RUN: diff %t1.ast.json %s.json

// Different tests to test that only the right constexpr function templates
// are reported.
//

// Should be reported, ticks all boxes
template<typename T>
constexpr T pi(){
    T data = T(3.14);
    return data;
}

// Should be reported, no problem that is doesn't declare any variables
template<typename T>
constexpr T pi2(){
    return T(3.14);
}

// Shouldn't be reported, wrong return type, empty return stmt.
template<typename T>
constexpr void pi3(){
    return;
}

// Invalid C++ code, but would not be reported due to emtpy return stmt.
// template<typename T>
// constexpr int pi4(){
//     return;
// }
