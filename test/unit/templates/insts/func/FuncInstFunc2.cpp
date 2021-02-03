// RUN: rm %t1.ast.json || true
// RUN: clang++ %s -emit-ast -o %t1.ast
// RUN: %S/../../../../../build/cxx-langstat --analyses=tia -emit-features -in %t1.ast -out %t1.ast.json --
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
