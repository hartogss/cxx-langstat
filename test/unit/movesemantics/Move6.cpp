// RUN: rm %t1.ast.json || true
// RUN: %clangxx %s -emit-ast -o %t1.ast -std=c++17
// RUN: %cxx-langstat --analyses=msa -emit-features -in %t1.ast -out %t1.ast.json -- -std=c++17
// RUN: diff %t1.ast.json %s.json

// Test that alias templates form no barrier to MSA.
//
//

#include <utility>
#include <iostream>

template<typename T>
struct C{
public:
    C(){std::cout << "ctor\n";}
};

template<typename T>
using D = C<T>;

template<typename T>
void func(T c){
}


int main(int argc, char** argv){
    C<int> c;
    D<int> d;
    // These should all result in copy ctor calls
    func(c);
    func(d);
}
