// RUN: rm %t1.ast.json || true
// RUN: %clangxx %s -emit-ast -o %t1.ast
// RUN: %cxx-langstat --analyses=fpa -emit-features -in %t1.ast -out %t1.ast.json --
// RUN: diff %t1.ast.json %s.json

// Basic test to check if params of function template specializations are reported.
// Function templates can only be fully specialized.
//

template<typename T>
void func(T t);

// explicit (full) specialization
template<>
void func<int>(int t);

// function templates cannot be partially specialized
