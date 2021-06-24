// RUN: rm %t1.ast.json || true
// RUN: %clangxx %s -emit-ast -o %t1.ast
// RUN: %cxx-langstat --analyses=cea -emit-features -in %t1.ast -out %t1.ast.json --
// RUN: diff %t1.ast.json %s.json

// Test to ensure that varDecls/parmVarDecls that have alias type are reported with the
// type that they alias s.t. we can possibly compare them when needed.
//

#include <vector>

struct C {};

using D = C;

// For some reason, the const-ness in the canonical type of the function is stripped away.
constexpr void func(const D d);

void func2(const std::vector<D>&& v);


struct E {
    // this just takes a struct C, const doesn't matter since we're passing by value
    constexpr void method(D d);
};

int main(int argc, char** argv){ // Don't report ParmVarDecl, cannot be constexpr
    constexpr D d;
    const std::vector<D> v = {};
}
