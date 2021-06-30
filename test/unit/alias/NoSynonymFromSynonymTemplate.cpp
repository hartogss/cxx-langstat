// RUN: rm %t1.ast.json || true
// RUN: %clangxx %s -emit-ast -o %t1.ast
// RUN: %cxx-langstat --analyses=ua -emit-features -in %t1.ast -out %t1.ast.json --
// RUN: diff %t1.ast.json %s.json

// Instantiation of "typedef template" and alias template does not cause the
// typedef in the class specialization to be reported, just like the alias
// is not reported, because that isn't even created.
// According to cpprefernce, alias template instantiation directly causes
// instantiation of class template it aliases.

template<typename T>
class C{};

template<typename T>
using A1 = C<T>;
A1<int> a1;

template<typename T>
struct A2 {
    typedef C<T> type;
};
A2<int>::type a2;

// -> one "typedef template" and one alias template is reported, nothing else.
// they are, so to say, "completely" transparent w.r.t. the instantiations
