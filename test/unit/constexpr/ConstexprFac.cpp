// RUN: rm %t1.ast.json || true
// RUN: %clangxx %s -emit-ast -o %t1.ast
// RUN: %cxx-langstat --analyses=cea -emit-features -in %t1.ast -out %t1.ast.json --
// RUN: diff %t1.ast.json %s.json

// Don't ever use this code.
//
//

#include<iostream>

struct Fac{
    constexpr Fac() : res(1){}
    constexpr Fac(const int n){
        if (n>1){
            Fac f(n-1); // why this not constexpr?
            res = n*f.get();
        } else {
            constexpr Fac f;
            res = f.get();
        }
    }
    constexpr int get() const{
        return res;
    }
    int res;
};

int main(int argc, char** argv){
    constexpr Fac f(10);
    constexpr int x = f.get();
    std::cout << x << std::endl;
}
