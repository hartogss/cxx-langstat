// RUN: clang++ %s -emit-ast -o %t1.ast
// RUN: %S/../../build/cxx-langstat --analyses=tia --store %t1.ast --
// RUN: diff %t1.ast.json %s.json




template<typename T>
class Sub {};

template<typename T>
class Super {
    Sub<int> w;
};


// No instantiations for 'Sub', since it occurs only inside a template 'Super',
// which is never instantiated itself.
