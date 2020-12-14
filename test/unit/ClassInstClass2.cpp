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
    Sub<int> w;
};
template class Super<int>;
template class Super<float>;

// The explicit instantiations don't define data/variables, but define classes
// i.e. define what they should look like, it makes sense to have for each super
// instantiation a sub<int> instantiation.
