// RUN: rm %t1.ast.json || true
// RUN: %clangxx %s -emit-ast -o %t1.ast
// RUN: %cxx-langstat --analyses=tia -emit-features -in %t1.ast -out %t1.ast.json --
// RUN: diff %t1.ast.json %s.json

// Instantiations: f1<int>, which occurs no matter if f2 is instantiated or not.
//
//


template<typename T>
void f1(){
}

template<typename T>
void f2(){
    f1<int>();
}
