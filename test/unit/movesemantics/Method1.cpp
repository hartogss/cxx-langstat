// RUN: rm %t1.ast.json || true
// RUN: %clangxx %s -emit-ast -o %t1.ast -std=c++17
// RUN: %cxx-langstat --analyses=msa -emit-features -in %t1.ast -out %t1.ast.json -- -std=c++17
// RUN: diff %t1.ast.json %s.json

// Same as the Move*.cpp test, but with method/member functions instead of
// non-member functions.
//

#include <utility>

struct D{
    D();
    D(const D& other);
};

struct C{
    void method(D d);
};

int main(int argc, char** argv){
    C c;
    D d;
    // Cause copy-construction in both cases because explicit copy constructor
    // of d causes no-compiler-generated move-constructor of D.
    c.method(d);
    c.method(std::move(d));
}
