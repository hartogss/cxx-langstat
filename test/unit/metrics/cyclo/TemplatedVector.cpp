// RUN: rm %t1.ast.json || true
// RUN: clang++ %s -emit-ast -o %t1.ast
// RUN: %cxx-langstat --analyses=cca -emit-features -in %t1.ast -out %t1.ast.json --
// RUN: diff %t1.ast.json %s.json

// CCA fails on this test because it cannot determine the CFG of foo
// this is due to "vec" being templated.
// Should be fixed.

#include<vector>

template<typename T>
void foo(std::vector<T> vec){
    // for(auto it = vec.begin(); it<vec.end(); it++){} // causes no problems
    for(auto el : vec){

    }
}

// Also happens for other templated class types and member functions
