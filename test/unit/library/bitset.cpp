// RUN: rm %t1.ast.json || true
// RUN: clang++ %s -emit-ast -o %t1.ast
// RUN: %S/../../../build/cxx-langstat --analyses=ula -emit-features -in %t1.ast -out %t1.ast.json --
// RUN: diff %t1.ast.json %s.json

//
//
//

#include <bitset>

int func(){
    std::bitset<3> t;
}
