// RUN: rm %t1.ast.json || true
// RUN: clang++ %s -emit-ast -o %t1.ast -std=c++17
// RUN: %cxx-langstat --analyses=msa -emit-features -in %t1.ast -out %t1.ast.json -- -std=c++17
// RUN: diff %t1.ast.json %s.json

// Test to see if pointers are reported or not.
//
//

#include <utility>

class C{};

void func(C c);

void func_p(C* c);

int main(int argc, char** argv){
    C c;
    func(std::move(c));

    func_p(&c);

}
