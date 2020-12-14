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

template class Super<int>;
Super<int> s2;


// One Super<int>, Sub<int> instantiation for the explicit instantiation;
// One Super<int> for the implicit instantiation.
// If you only had the implicit instantiation, you would have both Super<int>
// Sub<int> still, because the compiler would have to create one version of super
// for int, and then still create another version for sub<int>;
