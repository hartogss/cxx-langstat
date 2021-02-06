// RUN: rm %t1.ast.json || true
// RUN: clang++ %s -emit-ast -o %t1.ast
// RUN: %S/../../../../../build/cxx-langstat --analyses=tia -emit-features -in %t1.ast -out %t1.ast.json --
// RUN: diff %t1.ast.json %s.json

// Test whether instantiations of typedef/alias templates are reported.
//
//

// Template we want to look for instantiations/uses of
template<typename T>
class C{};
C<int> c;

// Single alias template
template<typename T>
using D = C<T>;
D<bool> d;

// Double alias template
template<typename T>
using E = D<T>;
E<char> e;

// "Typedef template"
template<typename T>
struct D2 {
    typedef C<T> type;
};
D2<float>::type d2;

// Why does this work too?
// Because variables of type D3<int> will simply be variables of type
// const C<int>, which are also of type C<int>.
template<typename T>
using D3 = const C<T>;
D3<int> d3;

// Variables to type D4<int> will be variables of type C<const int>.
template<typename T>
using D4 = C<const T>;
D4<int> d4;
