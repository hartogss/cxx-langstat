// RUN: rm %t1.ast.json || true
// RUN: clang++ %s -emit-ast -std=c++17 -o %t1.ast
// RUN: %cxx-langstat --analyses=cea -emit-features -in %t1.ast -out %t1.ast.json -- -std=c++17
// RUN: diff %t1.ast.json %s.json

// Small test to ensure cxxDeductionGuideDecl don't appear in CEA as
// non-constexpr functions.
// Need C++17+.

// Copied from
// https://clang.llvm.org/doxygen/classclang_1_1CXXDeductionGuideDecl.html#details
template<typename T>
struct A {
    A();
    A(T);
};
A() -> A<int>;
