// RUN: clang++ %s -emit-ast -o %t1.ast
// RUN: %S/../../build/cxx-langstat --analyses=tia %t1.ast --
// RUN: diff %t1.ast.json %s.json

// Test what happens when instantiating class templates that instantiate
// classes inside them. The subtle point is that we don't want to count class
// instantiations that are inside class templates. We only count them inside
// the instantiation class template.



template<typename T>
class Sub {};

template<typename T>
class Super {
    Sub<T> w;
};

Super<int> s1;
Super<int> s2;

// The implicit instantiations define variables/data. The programmer has
// explicitly written Super<int> twice, so it should be counted twice.
// If, however, you "expanded" the code to contain the class s.t. it contained
// the class fully-written out - a non-template class - a variable sub<int>
// would only occur once.
