// RUN: clang++ %s -emit-ast -o %t1.ast
// RUN: %S/../../../../../build/cxx-langstat --analyses=tia --store %t1.ast --
// RUN: diff %t1.ast.json %s.json

// Instantiations: Super<int>, Super<float> as ClassTemplateSpecializationDecl.
// Sub<int>, Sub<float> as FieldDecls of above CTSDs.

template<typename T>
class Sub {};

template<typename T>
class Super {
    Sub<T> w;
};
template class Super<int>;
template class Super<float>;
