// RUN: rm %t1.ast.json || true
// RUN: clang++ %s -emit-ast -o %t1.ast
// RUN: %cxx-langstat --analyses=vta -emit-features -in %t1.ast -out %t1.ast.json --
// RUN: diff %t1.ast.json %s.json

// Test interaction between class templates and variable templates at class
// scope.
//

// static class template member var
template<typename T>
class A {
    static T data;
};

// class with variable template
class B {
    template<typename T>
    static T data;
};


// data should be a variable template, it's simply inside of a class template
template<typename T>
class C2 {
    template<typename U>
    static U data;
};

// Although intuitively I would make this to be a static class template member
// var, it is a variable template according to cppreference.com.
// At class scope, a variable template has to be static.
template<typename T>
class C1 {
    template<typename U>
    static T data; // notice the type here is T, not U
};
