// RUN: rm %t1.ast.json || true
// RUN: clang++ %s -emit-ast -o %t1.ast
// RUN: %cxx-langstat --analyses=ua -emit-features -in %t1.ast -out %t1.ast.json --
// RUN: diff %t1.ast.json %s.json

// Test that typedefs inside a class template specialization that DOES stem
// from a "typedef template" are not reported as "plain typedefs"
//

// typedef template
template<typename T>
struct S {
    typedef T type;
};
// using that typedef template
// creates a specialization
S<int>::type test;

// We want to match one typedef template and nothing else
