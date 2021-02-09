// RUN: rm %t1.ast.json || true
// RUN: clang++ %s -emit-ast -o %t1.ast
// RUN: %S/../../../build/cxx-langstat --analyses=fpa -emit-features -in %t1.ast -out %t1.ast.json --
// RUN: diff %t1.ast.json %s.json

// Basic test to check if params of function template instantiations are reported.
//
//

template<typename T>
void func1(T t){}
template void func1<int>(int);

template<int N>
void func2(int&& n){}
template void func2<3>(int&&);

template<typename T>
void func3(T&& t){}
template void func3<int&>(int&); // Thank you, reference collapsing :)
