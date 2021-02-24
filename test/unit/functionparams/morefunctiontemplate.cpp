// RUN: rm %t1.ast.json || true
// RUN: clang++ %s -emit-ast -o %t1.ast
// RUN: %cxx-langstat --analyses=fpa -emit-features -in %t1.ast -out %t1.ast.json --
// RUN: diff %t1.ast.json %s.json
// Extra tests to see if function templates' parameters are correctly matched.
// Tests that parameter packs are correctly handled and && are correctly
// detected as either rvalue ref or universal/forwarding ref.

#include <array>

template<typename... Ts>
void f1(Ts... ts1); // √ pass by value param pack

template<typename... Ts>
void f2(Ts&... ts2); // √ pass by lvalue ref param pack

template<typename... Ts>
void f3(const Ts&... ts3); // √

template<typename... Ts>
void f5(Ts&&... ts5); // √

template<typename T, typename U>
void f51(T&& a, U&& b); // √


// Example based on Scott Meyers: Effective Modern C++, p.166 (2015):
template<typename T>
class MyVector {
    // Rvalue ref, because in an instantiation of MyVector T will be deduced
    // during class instantiation, and not during the push_back method call,
    // meaning there is no type deduction at push_back.
    void push_back(T&& x); // √
    // universal reference, because Args is deduced at call of emplace_back
    template<typename... Args>
    void emplace_back(Args&&... args); // √
    // Universal reference, same as above
    template<typename U>
    void senseless1(U&& u); // √
    // Would be universal reference if had type&& form, but std::array ruins it
    template<typename U>
    void senseless2(std::array<U, 0>&& arru); // √
};

template<typename T>
struct a {
    template<typename U>
    void func(T&& t, U&& u); // √ Rvalue ref t, but universal ref u
};  // T is deduced during instantiation of a, U is deduced at func's call site

template<typename T>
void funco1(){
    // Rvalue ref since the type deduction happens at the funco1 call,
    // not the funci1 call.
    void funci1(T&& t); // √
}
template<typename T>
void funco2(T&& t){ // universal reference? yes
    void funci2(T&& t); // √ universal reference? no, same reasoning as above.
}
