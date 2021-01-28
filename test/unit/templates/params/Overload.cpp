// RUN: rm %t1.ast.json || true
// RUN: clang++ %s -emit-ast -o %t1.ast
// RUN: %S/../../../../build/cxx-langstat --analyses=tpa -emit-features -in %t1.ast -out %t1.ast.json --
// RUN: diff %t1.ast.json %s.json

// Test to make sure that all overloads' template paramaters are analyzed
// & output as features.
//

// Function templates
template<typename T>
void f();

template<int N>
void f();
