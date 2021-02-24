// RUN: rm %t1.ast.json || true
// RUN: clang++ %s -emit-ast -o %t1.ast
// RUN: %cxx-langstat --analyses=ala,msa -emit-features -in %t1.ast -out %t1.ast.json --
// RUN: diff %t1.ast.json %s.json

// Test to check the different kinds of std::move are correctly distinguished.
//
//

#include "MoveVsMove2.h"

template<typename T>
void func(){}

int main(int argc, char** argv){
    //
    std::vector<int> v;
    std::list<int> l;
    std::move(v.begin(), v.end(), std::back_inserter(l));
    //
    return std::move(0);

}
