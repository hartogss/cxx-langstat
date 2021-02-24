// RUN: rm %t1.ast.json || true
// RUN: clang++ %s -emit-ast -o %t1.ast -std=c++17
// RUN: %cxx-langstat --analyses=msa -emit-features -in %t1.ast -out %t1.ast.json -- -std=c++17
// RUN: diff %t1.ast.json %s.json

// Test checking if by-value of template type parameter type parameter's
// construction is reported correctly.
//

#include <utility>
#include <iostream>

class C{
public:
    C(){std::cout << "ctor\n";}
    C(const C& c){std::cout << "copy ctor\n";}
    C(C&& c){std::cout << "move ctor\n";}
};

template<typename T>
void func(T c){
}


int main(int argc, char** argv){
    C c;
    func(c); // copy ctor
    func(std::move(c)); // move ctor

}
