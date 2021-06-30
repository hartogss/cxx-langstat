// RUN: rm %t1.ast.json || true
// RUN: %clangxx %s -emit-ast -o %t1.ast -std=c++17
// RUN: %cxx-langstat --analyses=msa -emit-features -in %t1.ast -out %t1.ast.json -- -std=c++17
// RUN: diff %t1.ast.json %s.json

// Basic test checking if by-value of record type parameters' construction
// is reported correctly.
// Requires C++17 in order not to incorrectly report mandatory elided moves.

#include <utility>
#include <iostream>

class C{
public:
    C(){std::cout << "ctor\n";}
    C(const C& c){std::cout << "copy ctor\n";}
    C(C&& c){std::cout << "move ctor\n";}
};

void func(C c){
}
void func1(C& c){
}

int main(int argc, char** argv){
    C c;
    func(c); // copy ctor
    func(C()); // Until C++17, it was allowed to create a temporary and to then
    // move from it. This move could potentially be elided.
    // Since C++17, the compiler is required to do "unmaterialized value passing".
    func(std::move(c)); // move ctor
    std::cout << std::endl;

    C d;
    C& e = d;
    func(e); // copy ctor
    func(std::move(e)); // move ctor
    std::cout << std::endl;

    // C f;
    // C&& g = f; can't bind to lvalue
    // Silently transformed to lvalue ref?
    // https://en.cppreference.com/w/cpp/language/reference
    // -> "rvalue reference vars are lvalues when used in exprs".
    C&& g = C();
    func(g);
    func(std::move(g));
    std::cout << std::endl;

    C h;
    func1(h); // no copy, just pass by reference
    // func1(std::move(h));
    std::cout << std::endl;

    C i;
    C& j = i;
    func1(j); // no copy, just pass by reference
    std::cout << std::endl;

}
