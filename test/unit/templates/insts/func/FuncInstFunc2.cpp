// RUN: clang++ %s -emit-ast -o %t1.ast
// RUN: %S/../../../../../build/cxx-langstat --analyses=tia --store %t1.ast --
// RUN: diff %t1.ast.json %s.json

// Instantiations: None, no inst of f2 causes no inst of f1.
//
//


template<typename T>
void f1(){
}

template<typename T>
void f2(){
    f1<T>();
}
