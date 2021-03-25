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

// "data" is a variable template, not a static class template member var.
// Look at the Clang AST.
// This is because "data" is still templated, even if the encompassing class
// is instantiated or specialized. Imagine the encompassing class had a non-type
// template parameter that would become the value assigned to data. If data is
// then still templated, it is still a variable template.
template<typename T>
class C1 {
    template<typename U>
    static T data; // notice the type here is T, not U
};
