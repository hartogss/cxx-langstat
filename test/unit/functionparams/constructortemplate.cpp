// RUN: rm %t1.ast.json || true
// RUN: %clangxx %s -emit-ast -o %t1.ast
// RUN: %cxx-langstat --analyses=fpa -emit-features -in %t1.ast -out %t1.ast.json --
// RUN: diff %t1.ast.json %s.json
// Test to ensure that constructors' and assignment operators' parameters
// are never looked at. Even when they're templated.
//

template<typename T>
class C{
    C()=default;
    C(int a){
    }
    C(const C& c)=default;
    C(C&& c)=default;
    ~C()=default;
};

class D{
    template<typename T>
    D(T t){

    }
    template<typename T> // perfect forwarding constructor
    D(T&& t){

    }
};

class E{
    template<template<typename T> class C>
    void operator=(const E& e);
    template<int N>
    void operator=(E&& e);

};

// This is invalid code, dtors cannot be templated
// class F{
//     template<typename U>
//     ~F(){
//
//     }
// };
