// RUN: rm %t1.ast.json || true
// RUN: %clangxx %s -emit-ast -o %t1.ast
// RUN: %cxx-langstat --analyses=tia -emit-features -in %t1.ast -out %t1.ast.json --
// RUN: diff %t1.ast.json %s.json

//
//
//

template<typename T>
T var;

template int var<int>;

void func(){
    var<bool>;
}

template float var<float>;
