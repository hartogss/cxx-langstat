// RUN: rm %t1.ast.json || true
// RUN: clang++ %s -emit-ast -o %t1.ast
// RUN: %cxx-langstat --analyses=cca -emit-features -in %t1.ast -out %t1.ast.json --
// RUN: diff %t1.ast.json %s.json

//
//
//

#include <vector>

// No problem computing CYC of both the template and the instantiation.
template<typename T>
void func(){
    int a = 3;

}
template void func<int>();

// Only manages to compute CYC for the instantiation because of the for-range loop.
template<typename T>
void test(T Matches){
    for(auto match : Matches){
    }
}
template void test<std::vector<int>>(std::vector<int>);

// Also able to do it.
template<typename T>
void test2(T Matches){
    for(int i=0; i<100; i++){
        Matches[i]=0;
    }
}
template void test2<int[42]>(int[42]);
