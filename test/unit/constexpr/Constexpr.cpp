// RUN: rm %t1.ast.json || true
// RUN: clang++ %s -emit-ast -o %t1.ast
// RUN: %S/../../../build/cxx-langstat --analyses=cea -emit-features -in %t1.ast -out %t1.ast.json --
// RUN: diff %t1.ast.json %s.json

// Basic tests to ensure constexpr is looked for only at the right places.
// Most notably, don't look at places and be like "there's no constexpr" for
// this variable when it isn't supported anyway by the language.


#include <utility>

template<typename T>
T var = 0;

template<typename T>
constexpr T varc = 0;


// (Constexpr) variable declarations and if statements
int main(int argc, char** argv){ // Don't report ParmVarDecl, cannot be constexpr
    // Var Decl
    constexpr int a = 0;
    int b = 1;
    auto [c,d] = std::make_pair(2,3); // Don't report DecompositionDecl, cannot be constexpr

    int x = var<int>;
    // constexpr int x = var<int>; // not possible
    int y = varc<int>;
    constexpr int yc = varc<int>;

    // If Stmt
    if constexpr(1 > 0){
    }

    if(1 < 0){
    }
}

struct C {
    constexpr C();
    C(int a) : a(a){}
    // constexpr ~C(); // Possible since C++20
private:
    int a;
};
