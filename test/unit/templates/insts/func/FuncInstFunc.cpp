// RUN: rm %t1.ast.json || true
// RUN: clang++ %s -emit-ast -o %t1.ast
// RUN: %cxx-langstat --analyses=tia -emit-features -in %t1.ast -out %t1.ast.json --
// RUN: diff %t1.ast.json %s.json
// Instantiations: f2<int>, f1<int>. f1 is only instantiated once f2 is.
//
//


template<typename T>
void f1(){
}

template<typename T>
void f2(){
    f1<T>(); // reported
}
template void f2<int>(); // not reported, as it is explicit
