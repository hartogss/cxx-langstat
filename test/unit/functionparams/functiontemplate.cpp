// RUN: rm %t1.ast.json || true
// RUN: clang++ %s -emit-ast -o %t1.ast
// RUN: %S/../../../build/cxx-langstat --analyses=msa -emit-features -in %t1.ast -out %t1.ast.json --
// RUN: diff %t1.ast.json %s.json
// Test to see if correctly for each kind of function template declaration
// parameter kind the right functions are found and all parmeters are listed
// correctly. Function templates need extra care because of universal references.

#include <vector>

class C{};

// template with no parameters at all, should never be matched
template<typename T>
void f();

// pass by value
template<typename T>
void f1(C c);
// pass by non-const lvalue reference
template<typename T>
void f2(C& c);
// pass by const lvalue reference
template<typename T>
void f3(const C& c);
// pass by rvalue reference
template<typename T>
void f4(C&& c);
// universal reference, because T is type deduced and has type&& form
template<typename T>
void f5(T&& param);
// rvalue reference because of qualifier
template<typename T>
void f5(const T&& param);
// rvalue ref, because not type&& form
template<typename T>
void f5(std::vector<T>&& param);
