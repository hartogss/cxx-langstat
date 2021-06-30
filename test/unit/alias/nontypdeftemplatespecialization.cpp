// RUN: rm %t1.ast.json || true
// RUN: %clangxx %s -emit-ast -o %t1.ast
// RUN: %cxx-langstat --analyses=ua -emit-features -in %t1.ast -out %t1.ast.json --
// RUN: diff %t1.ast.json %s.json

// Test to check that typedef inside a class template specialization that does
// NOT stem from a "typedef template" is reported.
//

// not typedef template, just a class template happening to have a typedef
template<typename T>
struct S {
    typedef T type;
    int x;
};
// Specializing above template will contain a typedef, we which
// want to match as a regular typedef
S<int>::type test;
// Explicit (full) specialization
template<>
struct S<double> {
    typedef double type;
};

// We want to match both the typedef in the class template and one typedef
// in its specialization caused by instantiation and in an explicit(full)
// specialization
